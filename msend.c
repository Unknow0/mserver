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
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

#define BUF_SIZE 1100

int main(int argc, char *argv[])
	{
	struct addrinfo *si;
	struct sockaddr_in sin;
	int s, i;
	char buf[BUF_SIZE];
	if(argc<3)
		{
		printf("usage: %s <host> <port> <message>+\n", argv[0]);
		return 1;
		}

	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		{
		printf("failed to open connection.\n");
		return 1;
		}
	
	if ((i=getaddrinfo(argv[1], argv[2], NULL, &si))!=0)
		{
		printf("getaddrinfo() failed: %s\n", gai_strerror(i));
		return 1;
		}
	char *b=buf;
	for(i=3; i<argc; i++)
		{
		sprintf(b, "%s ", argv[i]);
		b=b+1+strlen(argv[i]);
		}
	*(b-1)='\0';

	sin.sin_family=AF_INET;
	sin.sin_port=htons(0);
	sin.sin_addr.s_addr=inet_addr(argv[1]);

	if(bind(s, (const struct sockaddr*)&sin, sizeof(sin)))
		{
		perror("can't bind so can't get response");
		}
	
	printf("send '%s'\n", buf);
	if(sendto(s, buf, strlen(buf), 0, si->ai_addr, sizeof(*si->ai_addr))==-1)
		{
		printf("failed to send: %s\n", strerror(errno));
		return 1;
		}

	ssize_t size;
	if(strcmp(argv[3], "lib")==0)
		{
		unsigned short int *i;
		while((size=recvfrom(s, buf, BUF_SIZE, 0, NULL, NULL))>0)
			{
			buf[size]=0;
			i=(unsigned short *)buf;
			if(*i==157)
				break;
			}
		printf("%hu: ", *i);
		printf("%s\n", buf+2);
		}
/*	else
		{
		while((size=recvfrom(s, buf, BUF_SIZE, 0, NULL, NULL))>0)
			{
			buf[size]=0;
			printf("%s\n", buf);
			}
		}
*/	return 0;
	}
