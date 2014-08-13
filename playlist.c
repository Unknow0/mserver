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
#include <stdio.h>
#include <container/chunked_list.h>
#include "playlist.h"
#include "lib.h"

int flags;

int current=0;
char *next=NULL;
chunked_list_t *playlist;
chunked_list_t *filters;

void playlist_init()
	{
	playlist=chunked_list_create(512, sizeof(lib_entry));
	filters=chunked_list_create(10, sizeof(char*));

	playlist_reset();
	}

const char *playlist_next()
	{
	char *c;
	if(next!=NULL)
		{
		char *n=next;
		next=NULL;
		return n;
		}
	if(flags&PLAYLIST_RANDOM!=0)
		current=rand()%playlist->size;
	else
		current++;
	printf("get %d / %d\n", current, playlist->size);
	lib_entry *e=chunked_list_get(playlist, current);
	printf("%s: %d - %s\n", e->path, e->track, e->name);
	return e->path;
	}

void playlist_close()
	{
	}

void playlist_filter_add(char *f)
	{
	char *name=f;
	char *album=f;
	char *group=f;
	if(f[0]=='~')
		{
		if(f[1]=='n')
			name=f+2;
		else
			name=NULL;
		if(f[1]=='a')
			album=f+2;
		else
			album=NULL;
		if(f[1]=='g')
			group=f+2;
		else
			group=NULL;
		}

	lib_entry *e;
	iterator_t *it=chunked_list_iterator(playlist);
	while(iterator_has_next(it))
		{
		e=iterator_next(it);
		if(name!=NULL && !strcasestr(e->name, name))
			iterator_remove(it);
		else if(album!=NULL && !strcasestr(e->album, album))
			iterator_remove(it);
		else if(group!=NULL && !strcasestr(e->group, group))
			iterator_remove(it);
		}
	free(it);
	chunked_list_add(filters, f);
	}

void playlist_filter_del(char *f)
	{
	iterator_t *it=chunked_list_iterator(filters);
	while(iterator_has_next(it))
		{
		char *next=iterator_next(it);
		if(strcmp(next, f)==0)
			{
			iterator_remove(it);
			playlist_reset();
			break;
			}
		}
	free(it);
	}

int playlist_filter_match(lib_entry *e, char *f)
	{
	char *name=f;
	char *album=f;
	char *group=f;
	if(f[0]=='~')
		{
		if(f[1]=='n')
			name=f+2;
		else
			name=NULL;
		if(f[1]=='a')
			album=f+2;
		else
			album=NULL;
		if(f[1]=='g')
			group=f+2;
		else
			group=NULL;
		}
		return (name==NULL || strcasestr(e->name, name))
			 && (album==NULL || strcasestr(e->album, album))
			 && (group==NULL || strcasestr(e->group, group));
	}

void playlist_reset()
	{
	chunked_list_clear(playlist);

	iterator_t *i=chunked_list_iterator(filters);
	iterator_t *it=chunked_list_iterator(lib);
	while(iterator_has_next(it))
		{
		lib_entry *e=iterator_next(it);
		iterator_reset(i);
		while(iterator_has_next(i))
			{
			char *f=iterator_next(i);
			if(playlist_filter_match(e, f))
				chunked_list_add(playlist, iterator_next(it));
			}
		}
	free(i);
	free(it);
	}

void playlist_set_next(char *n)
	{
	next=n;
	}

void playlist_flags_set(int f)
	{
	flags|=f;
	}
void playlist_flags_unset(int f)
	{
	flags&=~f;
	}
void playlist_flags_toggle(int f)
	{
	flags^=f;
	}

void playlist_eof(player_t *p)
	{
	if((flags&PLAYLIST_SINGLE)==0)
		{
		player_play(p, playlist_next());
		}
	}
