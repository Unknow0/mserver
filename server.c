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
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <json/json.h>

#include <logger.h>

#include "lib.h"
#include "playlist.h"
#include "server.h"
#include "player.h"

#define BUFLEN 1024

int bcast_sock;

extern player_t *player;

logger_t *sl=NULL;
void server_parse_msg(int sock, unsigned char *buf, ssize_t len, struct sockaddr *si, int silen)
	{
	buf[len]=0;
	printf("%d: %s\n", len, buf);

	if(strncmp("resume", buf, 6)==0)
		player_resume(player);
	else if(strncmp("pause", buf, 5)==0)
		player_pause(player);
	else if(strncmp("stop", buf, 5)==0)
		player_stop(player);
	else if(strncmp("next", buf, 4)==0)
		player_next(player);
	else if(strncmp("play", buf, 4)==0)
		{
		if(len<=5)
			player_next(player);
		else
			player_play(player, buf+5);
		}
	else if(strncmp("set ", buf, 4)==0)
		{
		buf=buf+4;
		if(strncmp("random", buf, 6)==0)
			playlist_flags_set(PLAYLIST_RANDOM);
		else if(strncmp("repeat", buf, 6)==0)
			playlist_flags_set(PLAYLIST_REPEAT);
		}
	else if(strncmp("lib", buf, 3)==0)
		{
		char *b=malloc(2+lib_str->chunk_size);
		struct str_chunk *c=lib_str->head;
		unsigned short int i=0;
		while(c!=NULL)
			{
			memcpy(b, &i, 2);
			memcpy(b+2, c->data, c->len);
			sendto(sock, b, c->len+2, 0, si, silen);
			c=c->next;
			i++;
			}
		debug(sl, "writed %hu chunk", i);
		}
	printf("done server_parse_msg\n");
	}

void server_deinit(server_t *s)
	{
	info(sl, "Server shuting down");
	s->running=0;
	shutdown(s->sock, SHUT_RDWR);
	close(s->sock);
	}

server_t *server_init(unsigned short int port)
	{
	struct sockaddr_in name;
	server_t *s=malloc(sizeof(server_t));
	if(s==NULL)
		return NULL;
	sl=get_logger("mserver.server");
	s->sock = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s->sock < 0)
		{
		error(sl, "socket: %m");
		free(s);
		return NULL;
		}
	int broadcastEnable=1;
	setsockopt(s->sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

	bcast_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(bcast_sock<0)
		{
		perror("broadcast");
		return NULL;
		}

	s->bcast_addr.sin_family = AF_INET;
    s->bcast_addr.sin_port = htons(port);
    s->bcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	name.sin_family = AF_INET;
	name.sin_port = htons (port);
	name.sin_addr.s_addr = htonl (INADDR_ANY);
	if (bind (s->sock, (struct sockaddr *) &name, sizeof (name)) < 0)
		{
		error(sl, "bind: %m");
		free(s);
		return NULL;
		}
	s->running=1;
	return s;
	}

void server_mainloop(server_t *s)
	{
	int i;
	ssize_t size;
	struct sockaddr si;
	unsigned char buf[BUFLEN];
	while(s->running)
		{
		int slen=sizeof(si);
		info(sl, "waiting...");
		size=recvfrom(s->sock, buf, BUFLEN, 0, &si, &slen);
		if(size<0)
			{
			error(sl, "recvfrom(): %m");
			continue;
			}
		server_parse_msg(s->sock, buf, size, &si, slen);
		}
	}

void broadcast(server_t *s, const void *buf, int len)
	{
	if(sendto(s->sock, buf, len, 0, (const struct sockaddr*)&s->bcast_addr, sizeof(s->bcast_addr))<0)
		perror("broadcast");
	}
