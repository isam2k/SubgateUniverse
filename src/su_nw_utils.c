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

#define DEF_PORT_NUM "6249"
#define FLAG_ACK 0x00000001
#define FLAG_REF 0x00000002
#define FLAG_UPD 0x00000004
#define FLAG_INI 0x00000008

/*	*** FUNCTION DECLARATIONS *** */

void fnShowUsage(char *pname, char *msg)
{
	printf("%s:\n", msg);
	printf("USAGE:\n");
	printf("\t%s [dest_addr]\n", pname);
} // fnShowUsage

int fnInitSocket(int argc, char *argv[], struct sockaddr *dest_addr, socklen_t *addrlen)
{
	int					sockfd, state;
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
	uint32_t buffer[9], *pBuffer;
	struct timeval tv;
	fd_set readfds;
	
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	
	buffer[7] = htonl(pPlayer->iShipType);
	buffer[8] = htonl(FLAG_INI);
	
	left = sizeof(uint32_t) * 9;
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
		left = sizeof(uint32_t) * 9;
		pBuffer = &buffer[0];
		while (left > 0)
		{
			if ((sent = recvfrom(sockfd, pBuffer, left, 0, &dest_addr, &addrlen)) < 0)
				return 1;
			left -= sent;
			pBuffer += sent;
		} // while
		
		if (ntohl(buffer[8]) == (FLAG_INI | FLAG_ACK))
		{
			pPlayer->iPlayerId = ntohl(buffer[0]);
			pPlayer->fXPos = ntohl(*((float *)(&buffer[1])));
			pPlayer->fYPos = ntohl(*((float *)(&buffer[2])));
			pPlayer->fRotate = ntohl(*((float *)(&buffer[3])));
			pPlayer->fRotation = ntohl(*((float *)(&buffer[4])));
			pPlayer->fXAcceleration = ntohl(*((float *)(&buffer[5])));
			pPlayer->fXAcceleration = ntohl(*((float *)(&buffer[6])));
			pPlayer->iShipType = ntohl(buffer[7]);
		} // if player was accepted
		else
			return 1;
	} // if
	else
		return 1;
	return 0;
} //fnConnGameServer

int fnSendGameState(player_t *pPlayer, int sockfd, struct sockaddr dest_addr, socklen_t addrlen)
{
	ssize_t sent;
	size_t left;
	uint32_t buffer[9], *pBuffer;
	
	buffer[0] = htonl(pPlayer->iPlayerId);
	buffer[1] = htonl(*((uint32_t *)(&pPlayer->fXPos)));
	buffer[2] = htonl(*((uint32_t *)(&pPlayer->fYPos)));
	buffer[3] = htonl(*((uint32_t *)(&pPlayer->fRotate)));
	buffer[4] = htonl(*((uint32_t *)(&pPlayer->fRotation)));
	buffer[5] = htonl(*((uint32_t *)(&pPlayer->fXAcceleration)));
	buffer[6] = htonl(*((uint32_t *)(&pPlayer->fYAcceleration)));
	buffer[7] = htonl(pPlayer->iShipType);
	buffer[8] = htonl(FLAG_UPD);
	
	left = sizeof(uint32_t) * 9;
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

int fnGetUpdates(map_t *pMap, int sockfd)
{
	ssize_t sent;
	size_t left;
	uint32_t buffer[9], *pBuffer;
	struct timeval tv;
	fd_set readfds;
	player_t *pCurrent;
	
	tv.tv_sec = 0;		// polling updates
	tv.tv_usec = 0;
	
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	
	select(sockfd + 1, &readfds, NULL, NULL, &tv);
	while (FD_ISSET(sockfd, &readfds))
	{
		left = sizeof(uint32_t) * 9;
		pBuffer = &buffer[0];
		while (left > 0)
		{
			if ((sent = recvfrom(sockfd, pBuffer, left, 0, NULL, NULL)) < 0)
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
		
		if (buffer[8] == FLAG_UPD)
		{
			for (pCurrent = pMap->pOpponents; pCurrent != NULL; pCurrent = pCurrent->pNext)
			{
				if (pCurrent->iPlayerId == buffer[0]) break;
			} // if
			
			if (pCurrent != NULL)
			{
				pCurrent->fXPos = *((float *)(&buffer[1]));
				pCurrent->fYPos = *((float *)(&buffer[2]));
				pCurrent->fRotate = *((float *)(&buffer[3]));
				pCurrent->fAccelerate = 0;
				pCurrent->fRotation = *((float *)(&buffer[4]));
				pCurrent->fXAcceleration = *((float *)(&buffer[5]));
				pCurrent->fXAcceleration = *((float *)(&buffer[6]));
				pCurrent->iShipType = buffer[7];
			} // if
			else
			{
				pCurrent = malloc(sizeof(player_t));
				pCurrent->iPlayerId = buffer[0];
				pCurrent->fXPos = *((float *)(&buffer[1]));
				pCurrent->fYPos = *((float *)(&buffer[2]));
				pCurrent->fRotate = *((float *)(&buffer[3]));
				pCurrent->fAccelerate = 0;
				pCurrent->fRotation = *((float *)(&buffer[4]));
				pCurrent->fXAcceleration = *((float *)(&buffer[5]));
				pCurrent->fXAcceleration = *((float *)(&buffer[6]));
				pCurrent->iShipType = buffer[7];
				pCurrent->pNext = pMap->pOpponents;
				pMap->pOpponents = pCurrent;
			} // else
		} // if
		
		select(sockfd + 1, &readfds, NULL, NULL, &tv);
	} // while
	return 0;
} // fnGetUpdates
