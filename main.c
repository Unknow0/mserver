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
#include <bcm_host.h>
#include <json/json.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#include <logger.h>
#include <cfg.h>

#include <container/string.h>

#include "server.h"
#include "player.h"
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
	bcm_host_init();
	logger_init();
	gst_init(&argc, &argv);
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

	if(lib_init(cfg_get_string("mserver.dbfile"), cfg_get_string("mserver.libdir")))
		{
		fatal(ml, "error while initializing in db");
		return 1;
		}
	lib_str_init();

	info(ml, "lib init done");
	player=player_init();
	if(player==NULL)
		{
		fatal(ml, "failed to init player");
		return 1;
		}
	player->onEOF=&playlist_eof;
	info(ml, "player init done\n");
	server=server_init(cfg_get_int("mserver.port"));

	info(ml, "starting server");
	server_mainloop(server);

	playlist_close();
	lib_deinit();
	player_destroy(player);
	bcm_host_deinit();
	return 0;
	}
