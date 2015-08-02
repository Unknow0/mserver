/*******************************************************************************
 * This file is part of mserver.
 *
 * mserver is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * mserver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with mserver; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 ******************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include <logger.h>
#include <container/iterator.h>
#include <container/watch.h>
#include "lib.h"

int watch_fd;

logger_t *ll=NULL;

int lib_write(lib_t *lib)
	{
	int fd;
	iterator_t *it;

	fd=open(lib->dbfile, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
	if(fd)
		{
		write(fd, &lib->entries->size, sizeof(size_t));
		it=chunked_list_iterator(lib->entries);
		while(iterator_has_next(it))
			{
			lib_entry *e=iterator_next(it);
			size_t len=strlen(e->path)+strlen(e->group->str)+strlen(e->album->str)+strlen(e->name)+4;
			write(fd, &len, sizeof(size_t));
			write(fd, e->path, strlen(e->path)+1);
			write(fd, e->group->str, e->group->len+1);
			write(fd, e->album->str, e->album->len+1);
			write(fd, e->name, strlen(e->name)+1);
			}
		free(it);
		close(fd);
		}
	debug(ll, "done reading libfile");
	}

int lib_read(lib_t *lib)
	{
	size_t buf_len=256;
	char *buf=malloc(buf_len);
	int fd=open(lib->dbfile, O_RDONLY);
	if(fd)
		{
		lib_entry e;
		size_t s, l;
		debug(ll, "reading lib");
		read(fd, &s, sizeof(size_t));
		while(s--)
			{
			char *b=buf;
			read(fd, &l, sizeof(size_t));
			if(l>buf_len)
				{
				char *t=realloc(buf, l);
				if(!t)
					return 1;
				// TODO check error
				buf_len=l;
				buf=t;
				b=buf;
				}
			read(fd, b, l);
			e.path=strdup(b);
			b+=strlen(e.path);
			e.group=string_create_unique(b);
			b+=e.group->len;
			e.album=string_create_unique(b);
			b+=e.album->len;
			e.name=strdup(b);
			chunked_list_add(lib->entries, &e);
			}
		close(fd);
		debug(ll, "done reading lib");
		}
	free(buf);
	return 0;
	}

void tag_reader(const char *name, const char *value, void* data)
	{
	unsigned int i;
	char *tmp;
	lib_entry *entry=(lib_entry*)data;
	
	if(strcmp(name, "title")==0)
		entry->name=strdup(value);
	else if(strcmp(name, "artist")==0)
		entry->group=string_create_unique(value);
	else if(strcmp(name, "album")==0)
		entry->album=string_create_unique(value);
	else if(strcmp(name, "track")==0)
		entry->track=atoi(value);
	}

int filecount;
string_t *empty;
void parse_file(lib_t *lib, char *path, lib_entry *entry)
	{
	size_t pathsize=strlen(path)-lib->base_path_size;
	debug(ll, "parse_file '%s'", path);
	entry->path=strdup(path+lib->base_path_size);
	entry->group=empty;
	entry->album=empty;
	entry->name=(char*)empty->str;
	entry->track=0;

	if(player_metadata(path, &tag_reader, entry))
		{
		int ret=chunked_list_add(lib->entries, entry);
		if(ret)
			{
			error(ll, "chunked_list_add returned: %d", ret);
			free(entry->path);
			}
		else
			{ // notify?
			}
		}
	else
		{
		warn(ll, "%s: invalid file", path);
		free(entry->path);
		}
	}

void parse_dir(lib_t *lib, char **name, size_t l, lib_entry *entry)
	{
	int s=strlen(*name);
	struct dirent *d;
	watch(lib->watch, *name);
	DIR *dir=opendir(*name);
	while((d=readdir(dir))!=NULL)
		{
		if(d->d_name[0]=='.')	// skip hidden file
			continue;
		int s2=strlen(d->d_name);
		if(d->d_type==DT_DIR)
			{
			if(l<s+s2+1)
				{
				l=s+s2+1;
				*name=realloc(*name, l+1);
				}
			memcpy(*name+s, d->d_name, s2+1);
			(*name)[s+s2]='/'; (*name)[s+s2+1]=0;
			debug(ll, "parse_dir(%s)", *name);
			parse_dir(lib, name, l, entry);
			debug(ll, "return %s", *name);
			}
		else
			{
			filecount++;
			if(l<s+s2)
				{
				l=s+s2;
				*name=realloc(*name, l+1);
				}
			memcpy(*name+s, d->d_name, s2+1);
			(*name)[s+s2]=0;
			// read file metadata
			parse_file(lib, *name, entry);
			}
		}
	info(ll, "parsed %d files, %d keept", filecount, lib->entries->size);
	closedir(dir);
	}

char *lib_canonize(lib_t *lib, const char *f)
	{
	size_t s=strlen(f)+lib->base_path_size+1;
	char *ret=malloc(s);
	ret[0]=0;
	strcat(ret, lib->base_path);
	strcat(ret, f);
	return ret;
	}

void *check_lib(void *arg)
	{
	size_t l;
	char *n;
	lib_entry *entry;
	lib_t *lib=(lib_t *)arg;
	iterator_t *it=chunked_list_iterator(lib->entries);
	info(ll, "starting checker");
	while(iterator_has_next(it))
		{
		lib_entry *e=iterator_next(it);
		char *f=lib_canonize(lib, e->path);
		debug(ll, "cheking '%s'", f);
		if(access(f, R_OK))
			iterator_remove(it);
		free(f);
		}

	l=lib->base_path_size+1;
	n=malloc(l);
	memcpy(n, lib->base_path, l);

	entry=malloc(sizeof(lib_entry));

	parse_dir(lib, &n, l, entry);
	free(n);
	free(entry);
	}

void lib_destroy(lib_t *lib)
	{
	info(ll, "lib closing");
	lib_write(lib);
	
	chunked_list_destroy(lib->entries);
	free((void*)lib->base_path);

	string_destroy(empty);
	}

void lib_entry_destructor(void *v)
	{
	lib_entry *e=(lib_entry *)v;
	free(e->path);
	if(e->name!=empty->str)
		free(e->name);
	if(e->group!=empty)
		string_destroy(e->group);
	if(e->album!=empty)
		string_destroy(e->album);
	}

void lib_watch_event(struct inotify_event *e, const char *path, void *payload)
	{
	lib_entry entry;
	lib_t *lib=(lib_t*)payload;
	size_t l=strlen(path)+strlen(e->name)+1;
	char *f=malloc(l+1);
	strcpy(f, path);
	strcat(f, e->name);
	if(e->mask&IN_CLOSE_WRITE || e->mask&IN_CREATE || e->mask&IN_MOVED_TO)
		{
		struct stat buf;
		stat(f, &buf);
		if(S_ISDIR(buf.st_mode))
			{
			f[l-1]='/';
			f[l]=0;
			parse_dir(lib, &f, l, &entry);
			}
		else
			parse_file(lib, f, &entry);
		}
	else if(e->mask&IN_DELETE || e->mask&IN_DELETE_SELF || e->mask&IN_MOVED_FROM)
		{
		iterator_t *it=chunked_list_iterator(lib->entries);
		while(iterator_has_next(it))
			{
			lib_entry *e=iterator_next(it);
			if(strncmp(f, e->path, l-1)==0)
				iterator_remove(it);
			}
		}
	}

lib_t *lib_create(const char *dbfile, const char *libdir)
	{
	lib_t *lib=malloc(sizeof(lib_t));
	if(!lib)
		return NULL;
	lib->watch=watch_create(lib);
	if(!lib->watch)
		{
		error(ll, "failed to create watcher.");
		goto err;
		}
	lib->watch->event=lib_watch_event;

	empty=string_create_unique("");
	ll=get_logger("lib");
	lib->entries=chunked_list_create(512, sizeof(lib_entry), &lib_entry_destructor);
	if(!lib->entries)
		{
		error(ll, "failed to allocate lib.");
		goto err;
		}

	int s=strlen(libdir);
	if(libdir[s-1]!='/')
		{
		char *tmp=malloc(s+2);
		strcpy(tmp, libdir);
		tmp[s++]='/'; tmp[s]=0;
		libdir=tmp;
		}
	else
		libdir=strdup(libdir);
	lib->base_path=libdir;
	lib->base_path_size=s;
	filecount=0;

	
	lib->dbfile=strdup(dbfile);
	lib_read(lib);
	pthread_create(&lib->check_thread, NULL, check_lib, lib);
	pthread_join(lib->check_thread, NULL);
//	check_lib(lib);

	return lib;

err:
	if(lib)
		{
		if(lib->entries)
			chunked_list_destroy(lib->entries);
		if(lib->watch)
			watch_destroy(lib->watch);
		free(lib);
		}
	return NULL;
	}
