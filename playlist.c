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

void playlist_init()
	{
	playlist=chunked_list_create(512, sizeof(lib_entry));
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
		current=rand()%lib_count;
	else
		current++;
	printf("get %d / %d\n", current, lib_count);
	lib_entry *e=chunked_list_get(lib, current);
	printf("%s: %d - %s\n", e->path, e->track, e->name);
	return e->path;
	}

void playlist_close()
	{
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
