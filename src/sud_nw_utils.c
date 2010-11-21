/* *** INCLUDES *** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "sud_types.h"

/* *** DEFINES *** */

#define DEF_PORTNUM "6249"
#define DEF_MAXCONN 4
#define DEF_LOGFILE "sud.log"
#define FLAG_ACK 0x00000001
#define FLAG_MSG 0x00000002
#define FLAG_INI 0x00000004
#define FLAG_REF 0x00000008

/* *** FUNCTION DEFINITIONS *** */

void fnShowUsage(char *name, char *msg)
{
	printf("%s:\n", msg);
	printf("USAGE:\n");
	printf("\t%s [-l logfile] [-m maxconn] ...\n", name);
	printf("OPTIONS:\n");
	printf("\t[-l logfile]\tspecify a logfile\n");
	printf("\t[-m maxconn]\tlimit number of connected clients\n");
	printf("\t[-p port]\tbind socket to given port\n");
} // fnShowUsage

int fnInitServer(servstate_t *server_state, int argc, char *argv[])
{
	int					sockfd, state, i;
	struct addrinfo		hints, *serv_addr;
	
	state = -1;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	server_state->iMaxPlayers = DEF_MAXCONN;
	strcpy(server_state->LogFile, DEF_LOGFILE);
	
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-l") == 0)
		{
			if (argc > i + 1)
			{
				strcpy(server_state->LogFile, argv[i+1]);
				i++;
			} // if
			else
			{
				if (state != -1) freeaddrinfo(serv_addr);
				fnShowUsage(argv[0], "Insufficient arguments for option: -l");
				return -1;
			} // else
		} // if
		else if (strcmp(argv[i], "-m") == 0)
		{
			if (argc > i + 1)
			{
				server_state->iMaxPlayers = atoi(argv[i+1]);
				if (server_state->iMaxPlayers <= 0)
					return -1;
				i++;
			} // if
			else
			{
				if (state != -1) freeaddrinfo(serv_addr);
				fnShowUsage(argv[0], "Insufficient arguments for option: -m");
				return -1;
			} // else
		} // else if
		else if (strcmp(argv[i], "-p") == 0)
		{
			if (argc > i + 1 && state == -1)
			{
				if ((state = getaddrinfo(NULL, argv[i+1], &hints, &serv_addr)) != 0)
					return -1;
				i++;
			} // if
			else
			{
				fnShowUsage(argv[0], "Insufficient arguments for option: -p");
				return -1;
			} // else
		} // else if
		else
		{
			fnShowUsage(argv[0], "Unknown argument found.");
			return -1;
		} // else
	} // for
	
	if (state == -1)
	{
		if ((state = getaddrinfo(NULL, DEF_PORTNUM, &hints, &serv_addr)) != 0)
			return -1;
	} // if
	
	if ((sockfd = socket(serv_addr->ai_family, serv_addr->ai_socktype, serv_addr->ai_protocol)) < 0)
	{
		freeaddrinfo(serv_addr);
		return -1;
	} // if
	
	if (bind(sockfd, serv_addr->ai_addr, serv_addr->ai_addrlen) < 0)
	{
		freeaddrinfo(serv_addr);
		close(sockfd);
		return -1;
	} // if
	
	freeaddrinfo(serv_addr);
	
	server_state->iSockFd = sockfd;
	
	return 0;
} // fnInitServer

int fnRecvGameState(gamestate_t *rGs, struct sockaddr_storage *sourc_addr, socklen_t *addr_len, int sockfd)
{
	int i;
	size_t left;
	ssize_t read;
	uint32_t buffer[9], *pBuffer;
	
	*addr_len = sizeof(struct sockaddr_storage);
	
	left = sizeof(uint32_t) * 9;
	pBuffer = &buffer[0];
	while (left > 0)
	{
		if ((read = recvfrom(sockfd, pBuffer, left, 0, (struct sockaddr *)sourc_addr, addr_len)) < 0)
		{
			return 1;
		}
		else
		{
			left -= read;
			pBuffer += read;
		}
	} // while
	
	for (i = 0; i < 9; i++)
	{
		buffer[i] = ntohl(buffer[i]);
	} // for
	
	rGs->iPlayerId = buffer[0];
	rGs->fXPos = *((float *)(&buffer[1]));
	rGs->fYPos = *((float *)(&buffer[2]));
	rGs->fRotating = *((float *)(&buffer[3]));
	rGs->fRotation = *((float *)(&buffer[4]));
	rGs->fXAccel = *((float *)(&buffer[5]));
	rGs->fYAccel = *((float *)(&buffer[6]));
	rGs->iShipType = buffer[7];
	rGs->iHeading = buffer[8];
	return 0;
} // fnRecvGameState

int fnDistGameState(gamestate_t rGs, servstate_t *server_state)
{
	player_t *pCurrent;
	ssize_t sent;
	size_t left;
	uint32_t buffer[9], *pBuffer;
	int i;
	
	if (server_state->pPlayers == NULL)
		return 1;
	else
		pCurrent = server_state->pPlayers->pNext;
	
	buffer[0] = rGs.iPlayerId;
	buffer[1] = *((uint32_t *)(&rGs.fXPos));
	buffer[2] = *((uint32_t *)(&rGs.fYPos));
	buffer[3] = *((uint32_t *)(&rGs.fRotating));
	buffer[4] = *((uint32_t *)(&rGs.fRotation));
	buffer[5] = *((uint32_t *)(&rGs.fXAccel));
	buffer[6] = *((uint32_t *)(&rGs.fYAccel));
	buffer[7] = rGs.iShipType;
	buffer[8] = FLAG_MSG;
	
	for (i = 0; i < 9; i++)
	{
		buffer[i] = htonl(buffer[i]);
	} // for
	
	while (pCurrent != NULL)
	{
		if (pCurrent->iPlayerId == rGs.iPlayerId)
		{
			buffer[8] = buffer[8] | htonl(FLAG_ACK);
		} // if
		
		left = sizeof(uint32_t) * 9;
		pBuffer = &buffer[0];
		while (left > 0)
		{
			if ((sent = sendto(server_state->iSockFd, pBuffer, left, 0, (struct sockaddr *)&pCurrent->addr, pCurrent->addrlen)) < 0)
				return 1;
			left -= sent;
			pBuffer += sent;
		} // while
	
		pCurrent = pCurrent->pNext;
	} // while
	return 0;
} // fnDistGameState

int fnAckGameState(gamestate_t sGs, uint32_t flag, struct sockaddr_storage *sourc_addr, socklen_t addr_len, int sockfd)
{
	ssize_t sent;
	size_t left;
	uint32_t buffer[9], *pBuffer;
	
	buffer[0] = htonl(sGs.iPlayerId);
	buffer[8] = htonl(flag);
	
	left = sizeof(uint32_t) * 9;
	pBuffer = &buffer[0];
	while (left > 0)
	{
		if ((sent = sendto(sockfd, pBuffer, left, 0, (struct sockaddr *)sourc_addr, addr_len)) < 0)
			return 1;
		left -= sent;
		pBuffer += sent;
	} // while
	return 0;
} // fnAckGameState

player_t *fnMkPlayer(gamestate_t rGs, struct sockaddr_storage *sourc_addr, socklen_t *addr_len)
{
	player_t *pPlayer;
	
	if ((pPlayer = malloc(sizeof(player_t))) == NULL)
		return NULL;
	
	else
	{
		pPlayer->iPlayerId = rGs.iPlayerId;
		pPlayer->fXPos = rGs.fXPos;
		pPlayer->fYPos = rGs.fYPos;
		pPlayer->fRotating = rGs.fRotating;
		pPlayer->fRotation = rGs.fRotation;
		pPlayer->fXAccel = rGs.fXAccel;
		pPlayer->fYAccel = rGs.fYAccel;
		pPlayer->iShipType = rGs.iShipType;
		pPlayer->addr = *sourc_addr;
		pPlayer->addrlen = *addr_len;
		pPlayer->pPrev = NULL;
		pPlayer->pNext = NULL;
		return pPlayer;
	}
} // fnMkPlayer