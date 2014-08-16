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
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "player.h"
#include "lib.h"
#include "playlist.h"
#include "server.h"

static GstElement *metadata=NULL;

extern server_t *server;

void player_play(player_t *p, const char *file)
	{
	char *path=malloc(strlen(file)+strlen(lib_path)+1);
	strcpy(path, lib_path);
	strcat(path, file);

	gchar *uri=g_filename_to_uri(path, NULL, NULL);
	gst_element_set_state (p->play, GST_STATE_READY);
	g_object_set(G_OBJECT (p->play), "uri", uri, NULL);
	g_free (uri);
	free(path);
	gst_element_set_state (p->play, GST_STATE_PLAYING);
	}
void player_next(player_t *p)
	{
	player_play(p, playlist_next());
	}
void player_pause(player_t *p)
	{
	if(GST_STATE(p->play)==GST_STATE_PLAYING)
		gst_element_set_state (p->play, GST_STATE_PAUSED);
	}
void player_resume(player_t *p)
	{
	if(GST_STATE(p->play)==GST_STATE_PAUSED)
		gst_element_set_state (p->play, GST_STATE_PLAYING);
	}
void player_stop(player_t *p)
	{
	gst_element_set_state (p->play, GST_STATE_NULL);
	}

void player_destroy (player_t *p)
	{
	g_main_loop_quit(p->loop);
	gst_element_set_state (p->play, GST_STATE_NULL);
	gst_object_unref (p->play);
	free(p);
	}

gboolean bus_cb(GstBus *bus, GstMessage *msg, gpointer data)
	{
	player_t *p=(player_t *)data;
	switch (GST_MESSAGE_TYPE (msg))
		{
		case GST_MESSAGE_EOS:
			if(p->onEOF!=NULL)
				p->onEOF(p);
			break;
		case GST_MESSAGE_STATE_CHANGED:
			{
			GstState state;
			const char *m;
			if(msg->src==GST_OBJECT(p->play))
				{
				gst_message_parse_state_changed(msg, NULL, &state, NULL);
				m=gst_element_state_get_name(state);
				_l("mserver", 0, "%s(%d): %s", m);
				broadcast(server, m, strlen(m));
				}
			break;
			}
		case GST_MESSAGE_ERROR:
			{
			gchar *debug;
			GError *err;
			gst_message_parse_error (msg, &err, NULL);
			g_warning("Error: %s\n", err->message);
			g_error_free (err);
			break;
			}
		}
	return TRUE;
	}

int player_metadata(char *file, void (*callback)(const GstTagList *, const gchar *, gpointer), void *data)
	{
	GstMessage *msg;
	gchar *uri=g_filename_to_uri(file, NULL, NULL);
	int ret=0;
	if(metadata==NULL)
		{
		GstElement *sink;
		metadata = gst_element_factory_make ("playbin", "tagger");
		sink = gst_element_factory_make ("fakesink", NULL);
		g_object_set (metadata, "audio-sink", sink, NULL);
		}

	g_object_set (metadata, "uri", uri, NULL);
	g_free(uri);

	gst_element_set_state (metadata, GST_STATE_PAUSED);

	GstTagList *tags = NULL;

	msg = gst_bus_timed_pop_filtered (GST_ELEMENT_BUS (metadata), GST_CLOCK_TIME_NONE, GST_MESSAGE_ASYNC_DONE | GST_MESSAGE_TAG | GST_MESSAGE_ERROR);
	if(GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR)
		ret=1;
	else if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_TAG)
		{
		gst_message_parse_tag (msg, &tags);

		gst_tag_list_foreach (tags, callback, data);
		gst_tag_list_unref (tags);
		}

	gst_message_unref(msg);
	gst_element_set_state(metadata, GST_STATE_NULL);
	return ret;
	}

player_t *player_init()
	{
	pthread_t t;
	GstBus *bus;
	player_t *p=malloc(sizeof(player_t));

	p->play = gst_element_factory_make ("playbin", "gst-player");

	if(!p->play)
		{
		g_printerr ("Can't create pipeline.\n");
		return NULL;
		}

	p->loop=g_main_loop_new(NULL, FALSE);
	bus = gst_pipeline_get_bus(GST_PIPELINE (p->play));
	gst_bus_add_watch(bus, bus_cb, p);
	gst_object_unref (bus);
	pthread_create(&t, NULL, (void *(*)(void*))g_main_loop_run, p->loop);
	return p;
	}
