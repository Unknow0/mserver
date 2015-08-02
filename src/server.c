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
#include <json-c/json.h>

#include <logger.h>
#include <libffplay.h>

#include "lib.h"
#include "playlist.h"
#include "server.h"

#define BUFLEN 1024

int bcast_sock;

extern player_t *player;

logger_t *sl=NULL;
char *status_buf;
size_t status_buf_len=0;
void server_parse_msg(int sock, unsigned char *buf, ssize_t len, struct sockaddr *si, int silen)
	{
	buf[len]=0;
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, si, str, INET_ADDRSTRLEN);
	debug(sl, "server_parse_msg(%d, %s, %d, %s)\n", sock, buf, len, str);

	if(strncmp("resume", buf, 6)==0)
		player_setstate(player, PLAYER_STATE_PLAY);
	else if(strncmp("pause", buf, 5)==0)
		player_setstate(player, PLAYER_STATE_PAUSE);
	else if(strncmp("stop", buf, 5)==0)
		player_setstate(player, PLAYER_STATE_STOP);
	else if(strncmp("next", buf, 4)==0)
		{
		char *file=lib_canonize(lib, playlist_next());
		player_play(player, file);
		free(file);
		}
	else if(strncmp("play", buf, 4)==0)
		{
		char *file;
		if(len<=5)
			file=lib_canonize(lib, playlist_next());
		else
			file=lib_canonize(lib, buf+5);
		player_play(player, file);
		free(file);
		}
	else if(strncmp("set ", buf, 4)==0)
		{
		buf=buf+4;
		if(strncmp("random", buf, 6)==0)
			playlist_flags_set(PLAYLIST_RANDOM);
		else if(strncmp("repeat", buf, 6)==0)
			playlist_flags_set(PLAYLIST_REPEAT);
		}
	else if(strncmp("filter", buf, 6)==0)
		{
		if(len>12)
			{
			if(strncmp("add", buf+7, 3)==0)
				playlist_filter_add(buf+11);
			else if(strncmp("del", buf+7, 3)==0)
				playlist_filter_del(buf+11);
			}
		}
	else if(strncmp("status", buf, 6)==0)
		{
		lib_entry *e=playlist_current();
		int len=snprintf(status_buf, status_buf_len, "%s - %s (%s)",  e->name, e->group->str, e->album->str);
		if(len>=status_buf_len)
			{
			char *tmp=realloc(status_buf, len+1);
			if(tmp)
				{
				status_buf=tmp;
				status_buf_len=len+1;
				snprintf(status_buf, status_buf_len, "%s - %s (%s)",  e->name, e->group->str, e->album->str);
				}
			else
				status_buf[0]=0;
			}
		sendto(sock, status_buf, len+1, 0, si, silen);
		}
	else if(strncmp("lib", buf, 3)==0)
		{
		struct str_chunk *c=lib->lib_str->head;
		char *b=malloc(2+lib->lib_str->chunk_size);
		unsigned short int i=0;
		while(c!=NULL)
			{
			i++;
			c=c->next;
			}
		c=lib->lib_str->head;
		if(len>4) // index asked
			{
			int off=atoi(buf+4);
			while(c!=NULL && i>off)
				{
				c=c->next;
				i--;
				}
			memcpy(b, &i, 2);
			if(c!=NULL)
				{
				memcpy(b+2, c->data, c->len);
				sendto(sock, b, c->len+2, 0, si, silen);
				}
			else
				sendto(sock, b, 2, 0, si, silen);
			}
		else	// send everything
			{
			while(c!=NULL)
				{
				i--;
				memcpy(b, &i, 2);
				memcpy(b+2, c->data, c->len);
				sendto(sock, b, c->len+2, 0, si, silen);
				c=c->next;
				}
			}
		}
	}

void server_deinit(server_t *s)
	{
	info(sl, "Server shuting down");
	s->running=0;
	shutdown(s->sock, SHUT_RDWR);
	close(s->sock);
	free(s);
	}

server_t *server_init(unsigned short int port)
	{
	struct sockaddr_in name;
	server_t *s=malloc(sizeof(server_t));
	if(s==NULL)
		return NULL;
	sl=get_logger("server");
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
	name.sin_addr.s_addr = htonl(INADDR_ANY);
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
