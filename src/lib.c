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
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <logger.h>
#include "lib.h"

chunked_list_t *lib;
const char *lib_path;
size_t lib_path_size;
size_t lib_count=0;

char *lib_dbfile;

logger_t *ll=NULL;

int lib_write()
	{
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

static char *n;
static size_t l=0;
int filecount;
lib_entry *entry;
string_t *empty;
void parse_dir(const char *name)
	{
	debug(ll, "parse_dir(%s)", name);
	int s=strlen(name);
	struct dirent *d;
	if(l==0)
		{
		l=s+1;
		n=malloc(l+1);
		memcpy(n, name, l);
		}
	DIR *dir=opendir(name);
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
				n=realloc(n, l+1);
				}
			memcpy(n+s, d->d_name, s2+1);
			n[s+s2]='/'; n[s+s2+1]=0;
			parse_dir(n);
			}
		else
			{
			filecount++;
			if(l<s+s2)
				{
				l=s+s2;
				n=realloc(n, l+1);
				}
			memcpy(n+s, d->d_name, s2+1);
			n[s+s2]=0;
			// read file metadata
			size_t pathsize=strlen(n)-strlen(lib_path);
			entry->path=strdup(n+strlen(lib_path));
			entry->group=empty;
			entry->album=empty;
			entry->name=(char*)empty->str;
			entry->track=0;

			if(player_metadata(n, &tag_reader, entry))
				{
				int ret=chunked_list_add(lib, entry);
				if(ret)
					{
					error(ll, "chunked_list_add returned: %d", ret);
					free(entry->path);
					}
				else
					lib_count++;
				}
			else
				{
				warn(ll, "%s: invalid file", n);
				free(entry->path);
				}
			}
		}
	info(ll, "parsed %d files, %d keept", filecount, lib_count);
	closedir(dir);
	}

const char *lib_canonize(const char *f)
	{
	size_t s=strlen(f)+lib_path_size+1;
	if(s>l)
		{
		l=s;
		n=realloc(n, l);
		}
	n[lib_path_size]=0;
	strcat(n, f);
	return n;
	}

void lib_deinit()
	{
	info(ll, "lib closing");
	
	chunked_list_destroy(lib);
	free((void*)lib_path);
	free(n);

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

int lib_init(const char *dbfile, const char *libdir)
	{
	lib_count=0;
	empty=string_create_unique("");
	ll=get_logger("mserver.lib");
	lib=chunked_list_create(512, sizeof(lib_entry), &lib_entry_destructor);
	if(lib==0)
		{
		error(ll, "failed to allocate lib");
		return 1;
		}

	entry=malloc(sizeof(lib_entry));
	int s=strlen(libdir);
	if(libdir[s-1]!='/')
		{
		char *tmp=malloc(s+2);
		strcpy(tmp, libdir);
		tmp[s]='/', tmp[s+1]=0;
		libdir=tmp;
		}
	else
		libdir=strdup(libdir);
	lib_path=libdir;
	lib_path_size=strlen(lib_path);
	filecount=0;
	parse_dir(libdir);
	free(entry);

	return 0;
	}
