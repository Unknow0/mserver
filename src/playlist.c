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
#include <utils/chunked_list.h>
#include "playlist.h"
#include "lcg.h"

int flags;

int current=0;
char *next=NULL;
chunked_list_t *playlist;
chunked_list_t *filters;

lcg_state *lcg;
size_t last;

void playlist_init()
	{
	playlist=chunked_list_create(512, sizeof(lib_entry), NULL);
	filters=chunked_list_create(10, sizeof(char*),NULL);

	playlist_reset();
	srandom(time(NULL));
	last=0;
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
	if(playlist->size==0)
		return NULL;
	if(flags&PLAYLIST_RANDOM)
		{
		current=lcg_next(lcg);
		if(current==last)
			lcg_reset(lcg, playlist->size);
		}
	else
		current++;
	if(current>=playlist->size)
		{
		current=0;
		return NULL;
		}
	lib_entry *e=chunked_list_get(playlist, current);
	return e->path;
	}

void playlist_deinit()
	{
	chunked_list_destroy(filters);
	chunked_list_destroy(playlist);
	}
#include <logger.h>
extern logger_t *ml;
void playlist_filter_add(char *f)
	{
	char *name=f;
	char *album=f;
	char *group=f;
	info(ml, "filter: '%s'", f);
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
		info(ml, "	%s %s %s", e->name, e->album->str, e->group->str);
		if((name!=NULL && strcasestr(e->name, name))
			 || (album!=NULL && strcasestr(e->album->str, album))
			 || (group!=NULL && strcasestr(e->group->str, group)))
			continue;
		iterator_remove(it);
		}
	free(it);
	chunked_list_add(filters, f);

	if(flags&PLAYLIST_RANDOM)
		{
		lcg=lcg_create(playlist->size);
		last=lcg_next(lcg);
		}
	}

void playlist_filter_del(char *f)
	{
	iterator_t *it=chunked_list_iterator(filters);
	while(iterator_has_next(it))
		{
		char *next=iterator_next(it);
		if(strcasecmp(next, f)==0)
			iterator_remove(it);
		}
	free(it);
	playlist_reset();
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
			 || (album==NULL || strcasestr(e->album->str, album))
			 || (group==NULL || strcasestr(e->group->str, group));
	}

void playlist_reset()
	{
	chunked_list_clear(playlist);

	iterator_t *i=chunked_list_iterator(filters);
	iterator_t *it=chunked_list_iterator(lib->entries);
	while(iterator_has_next(it))
		{
		lib_entry *e=iterator_next(it);
		if(filters->size>0)
			{
			int ok=1;
			iterator_reset(i);
			while(iterator_has_next(i))
				{
				char *f=iterator_next(i);
				ok&=playlist_filter_match(e, f);
				}
			if(ok)
				chunked_list_add(playlist, iterator_next(it));
			}
		else
			chunked_list_add(playlist, e);
		}
	free(i);
	free(it);

	if(flags&PLAYLIST_RANDOM)
		{
		lcg=lcg_create(playlist->size);
		last=lcg_next(lcg);
		}
	}

void playlist_set_next(char *n)
	{
	next=n;
	}

void playlist_flags_set(int f)
	{
	flags|=f;
	if(flags&PLAYLIST_RANDOM)
		{
		lcg=lcg_create(playlist->size);
		last=lcg_next(lcg);
		}
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
		char *f=lib_canonize(lib, playlist_next());
		player_play(p, f);
		free(f);
		}
	}

lib_entry *playlist_current()
	{
	return chunked_list_get(playlist, current);
	}
