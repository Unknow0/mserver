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

#include "config.h"
#ifdef HAVE_LIBBCM_HOST
#	include <bcm_host.h>
#endif
#include <json/json.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>

#include <logger.h>
#include <cfg.h>
#include <container/string.h>
#include <libffplay.h>

#include "server.h"
#include "lib.h"
#include "playlist.h"

player_t *player;
server_t *server;

logger_t *ml=NULL;

void close_handler(int sig)
	{
	server->running=0;
	info(ml, "kill");
	}

int main(int argc, char *argv[])
	{
	struct sigaction act;
#ifdef HAVE_LIBBCM_HOST
	bcm_host_init();
#endif
	logger_init();

	av_log_set_level(40);
	av_register_all();
	avdevice_register_all();
	avcodec_register_all();
	avfilter_register_all();

	string_init();
	ml=get_logger("mserver");
	info(ml, "Starting mserver");
	srand(time(NULL));

	act.sa_handler=&close_handler;
	sigaction(SIGHUP, &act, NULL);
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGQUIT, &act, NULL);
	sigaction(SIGABRT, &act, NULL);
	sigaction(SIGPIPE, &act, NULL);
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGTSTP, &act, NULL);
	sigaction(SIGTTIN, &act, NULL);
	sigaction(SIGTTOU, &act, NULL);

	if(!cfg_has_key("mserver.libdir"))
		{
		fatal(ml, "Cfg 'mserver.libdir' should exist!");
		return 1;
		}
	if(!cfg_has_key("mserver.dbfile"))
		{
		fatal(ml, "Cfg 'mserver.dbfile' should exist!");
		return 1;
		}

	char *fmt=cfg_get_string("mserver.audio_fmt");
	char *file=cfg_get_string("mserver.audio_file");
	player=player_init(file, fmt);
	if(!player)
		{
		fatal(ml, "failed to init player");
		return 1;
		}
	info(ml, "player init done\n");

	if(lib_init(cfg_get_string("mserver.dbfile"), cfg_get_string("mserver.libdir")))
		{
		fatal(ml, "error while initializing in db");
		return 1;
		}
	lib_str_init();
	playlist_init();
	player->onEof=&playlist_eof;

	info(ml, "lib init done");
	server=server_init(cfg_get_int("mserver.port"));
	if(!server)
		{
		error(ml, "failed to init server");
		return 1;
		}
	info(ml, "starting server");
	server_mainloop(server);

	playlist_close();
	lib_deinit();
	player_destroy(player);
	bcm_host_deinit();
	return 0;
	}
