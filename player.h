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
#include <gst/gst.h>

#ifndef _PLAYER_H
#define _PLAYER_H
typedef struct player
	{
	GstElement *play;
	GstElement *metadata;
	GMainLoop *loop;
	void (*onEOF)(struct player *p);
	} player_t;

player_t *player_init();

void player_play(player_t *player, const char *);
void player_next(player_t *player);
void player_pause(player_t *player);
void player_resume(player_t *player);
void player_stop(player_t *player);

void player_destroy(player_t *player);

int player_metadata(char *file, void (*callback)(const GstTagList *, const gchar *, gpointer), void *data);

#endif
