/*
 *					~~~ SUBGATE UNIVERSE ~~~
 *	Subgate Universe is a 3D multiplayer space shooter. The project embraces a server, as well
 *	as a client application.
 *
 *	Copyright (C) 2010 Aeberhard Samuel
 *
 *	This program is free software; you can redistribute it and/or modify it under the terms of
 *	the GNU General Public License as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *	
 *	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *	without even the implied warranty of MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *	See the GNU General Public License for more details.
 *
 *	You should have recieved a copy of the GNU General Public License along with this program;
 *	if not, see
 *	<http://www.gnu.org/licenses/>
 */

/* *** INCLUDES *** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include "su_types.h"

/*	*** DEFINES *** */
#define PACK_SIZE 10
#define DEF_PORT_NUM "6249"
#define FLAG_ACK 0x20000000
#define FLAG_ENT 0x40000000
#define FLAG_INI 0x80000000

/*	*** FUNCTION DECLARATIONS *** */
void fnShowUsage(char *pname, char *msg)
{
	printf("%s:\n", msg);
	printf("USAGE:\n");
	printf("\t%s [dest_addr]\n", pname);
} // fnShowUsage

int fnInitSocket(int argc, char *argv[], struct sockaddr *dest_addr, socklen_t *addrlen)
{
	int			sockfd, state;
	struct addrinfo		hints, *serv_addr;
	
	if (argc < 2)
	{
		fnShowUsage(argv[0], "Insufficient arguments");
		return -1;
	} // if
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	
	if ((state = getaddrinfo(argv[1], DEF_PORT_NUM, &hints, &serv_addr)) != 0)
	{
		perror("getaddrinfo");
		return -1;
	}
	
	if ((sockfd = socket(serv_addr->ai_family, serv_addr->ai_socktype, serv_addr->ai_protocol)) < 0)
	{
		perror("socket");
		return -1;
	}
	
	*dest_addr = *((struct sockaddr *)serv_addr->ai_addr);
	*addrlen = serv_addr->ai_addrlen;
	
	freeaddrinfo(serv_addr);
	return sockfd;
} // fnInitSocket

int fnConnGameServer(player_t *pPlayer, int sockfd, struct sockaddr dest_addr, socklen_t addrlen)
{
	ssize_t sent;
	size_t left;
	uint32_t buffer[PACK_SIZE], *pBuffer;
	struct timeval tv;
	fd_set readfds;
	int i;
	
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	
	buffer[8] = htonl(pPlayer->iShipType);
	buffer[9] = htonl(FLAG_INI);
	
	left = sizeof(uint32_t) * PACK_SIZE;
	pBuffer = &buffer[0];
	while (left > 0)
	{
		if ((sent = sendto(sockfd, pBuffer, left, 0, &dest_addr, addrlen)) < 0)
			return 1;
		left -= sent;
		pBuffer += sent;
	} // while
	
	select(sockfd + 1, &readfds, NULL, NULL, &tv);
	
	if (FD_ISSET(sockfd, &readfds))
	{
		addrlen = sizeof(struct sockaddr);
		left = sizeof(uint32_t) * PACK_SIZE;
		pBuffer = &buffer[0];
		while (left > 0)
		{
			if ((sent = recvfrom(sockfd, pBuffer, left, 0, &dest_addr, &addrlen)) < 0)
				return 1;
			left -= sent;
			pBuffer += sent;
		} // while
		
		for (i = 0; i < PACK_SIZE; i++)
		{
			buffer[i] = ntohl(buffer[i]);
		} // for
		
		if ((buffer[9] & FLAG_ENT) > 0)
		{
			pPlayer->iPlayerId = buffer[0];
			pPlayer->fXPos = *((float *)(&buffer[1]));
			pPlayer->fYPos = *((float *)(&buffer[2]));
			pPlayer->fRotating = *((float *)(&buffer[3]));
			pPlayer->fRotation = *((float *)(&buffer[4]));
			pPlayer->fAccelerating = *((float *)(&buffer[5]));
			pPlayer->fXAcceleration = *((float *)(&buffer[6]));
			pPlayer->fXAcceleration = *((float *)(&buffer[7]));
			pPlayer->iShipType = buffer[8];
		} // if player was accepted
		else
			return 1;
	} // if
	else
		return 1;
	return 0;
} //fnConnGameServer

int fnSendGameState(player_t *pPlayer, uint32_t msg, int sockfd, struct sockaddr dest_addr, socklen_t addrlen)
{
	ssize_t sent;
	size_t left;
	uint32_t buffer[PACK_SIZE], *pBuffer;
	
	buffer[0] = htonl(pPlayer->iPlayerId);
	buffer[1] = htonl(*((uint32_t *)(&pPlayer->fXPos)));
	buffer[2] = htonl(*((uint32_t *)(&pPlayer->fYPos)));
	buffer[3] = htonl(*((uint32_t *)(&pPlayer->fRotating)));
	buffer[4] = htonl(*((uint32_t *)(&pPlayer->fRotation)));
	buffer[5] = htonl(*((uint32_t *)(&pPlayer->fAccelerating)));
	buffer[6] = htonl(*((uint32_t *)(&pPlayer->fXAcceleration)));
	buffer[7] = htonl(*((uint32_t *)(&pPlayer->fYAcceleration)));
	buffer[8] = htonl(pPlayer->iShipType);
	buffer[9] = htonl(msg);
	
	left = sizeof(uint32_t) * PACK_SIZE;
	pBuffer = &buffer[0];
	while (left > 0)
	{
		if ((sent = sendto(sockfd, pBuffer, left, 0, &dest_addr, addrlen)) < 0)
			return 1;
		left -= sent;
		pBuffer += sent;
	} // while
	return 0;
} // fnSendGameState

int fnGetUpdates(map_t *pMap, int sockfd, uint32_t *msg)
{
	ssize_t sent;
	size_t left;
	uint32_t buffer[PACK_SIZE], *pBuffer;
	struct timeval tv;
	fd_set readfds;
	player_t *pCurrent;
	struct sockaddr serv_addr;
	socklen_t addr_len;
	
	tv.tv_sec = 0;	// polling updates
	tv.tv_usec = 0;
	
	*msg = 0;
	
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	
	select(sockfd + 1, &readfds, NULL, NULL, &tv);
	while (FD_ISSET(sockfd, &readfds))
	{
		left = sizeof(uint32_t) * PACK_SIZE;
		pBuffer = &buffer[0];
		while (left > 0)
		{
			if ((sent = recvfrom(sockfd, pBuffer, left, 0, &serv_addr, &addr_len)) < 0)
				return 1;
			left -= sent;
			pBuffer += sent;
		} // while
		
		buffer[0] = ntohl(buffer[0]);
		buffer[1] = ntohl(buffer[1]);
		buffer[2] = ntohl(buffer[2]);
		buffer[3] = ntohl(buffer[3]);
		buffer[4] = ntohl(buffer[4]);
		buffer[5] = ntohl(buffer[5]);
		buffer[6] = ntohl(buffer[6]);
		buffer[7] = ntohl(buffer[7]);
		buffer[8] = ntohl(buffer[8]);
		buffer[9] = ntohl(buffer[9]);
		
		if (buffer[9] == FLAG_ACK)
		{
			
		} // if
		else
		{
			for (pCurrent = pMap->pOpponents; pCurrent != NULL; pCurrent = pCurrent->pNext)
			{
				if (pCurrent->iPlayerId == buffer[0]) break;
			} // if
			
			if (pCurrent == NULL || pCurrent->iPlayerId != buffer[0])
			{
				pCurrent = malloc(sizeof(player_t));
				pCurrent->iPlayerId = buffer[0];
				pCurrent->pNext = pMap->pOpponents;
				pMap->pOpponents = pCurrent;
			} // if
			
			pCurrent->fXPos = *((float *)(&buffer[1]));
			pCurrent->fYPos = *((float *)(&buffer[2]));
			pCurrent->fRotating = *((float *)(&buffer[3]));
			pCurrent->fRotation = *((float *)(&buffer[4]));
			pCurrent->fAccelerating = *((float *)(&buffer[5]));
			pCurrent->fXAcceleration = *((float *)(&buffer[6]));
			pCurrent->fYAcceleration = *((float *)(&buffer[7]));
			pCurrent->iShipType = buffer[8];
			
			*msg = FLAG_ACK;
		} // else
		
		select(sockfd + 1, &readfds, NULL, NULL, &tv);
	} // while
	return 0;
} // fnGetUpdates
