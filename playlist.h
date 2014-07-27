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
#ifndef _PLAYLIST_H
#define _PLAYLIST_H

#include "player.h"

#define PLAYLIST_RANDOM	(1<<0)
#define PLAYLIST_REPEAT (1<<1)
#define PLAYLIST_SINGLE (1<<2)

const char *playlist_next();

void playlist_set_next(char *next);

void playlist_flags_set(int flags);
void playlist_flags_unset(int flags);
void playlist_flags_toggle(int flags);

void playlist_eof(player_t *p);

void playlist_close();

#endif
