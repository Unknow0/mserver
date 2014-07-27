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
#ifndef _SERVER_H
#define _SERVER_H

#include <arpa/inet.h>

typedef struct server
	{
	int sock;
	int running;
	struct sockaddr_in bcast_addr;
	} server_t;

server_t *server_init(unsigned short int);
void server_deinit(server_t *);

void server_mainloop(server_t *);

void broadcast(server_t *s, const void *buf, int len);

#endif
