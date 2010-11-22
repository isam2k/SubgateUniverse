#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "su_types.h"

/*	*** DEFINES *** */

#define MAX_QUEUE 5
#define DEF_PORT_NUM "6249"

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

void fnSendGameState(player_t *pPlayer, int sockfd, struct sockaddr dest_addr, socklen_t addrlen)
{
	ssize_t sent;
	size_t left;
	uint32_t buffer[9], *pBuffer;
	int i;
	
	buffer[0] = 1;
	buffer[1] = *((uint32_t *)(&pPlayer->fXPos));
	buffer[2] = *((uint32_t *)(&pPlayer->fYPos));
	buffer[3] = *((uint32_t *)(&pPlayer->fRotate));
	buffer[4] = *((uint32_t *)(&pPlayer->fRotation));
	buffer[5] = *((uint32_t *)(&pPlayer->fXAcceleration));
	buffer[6] = *((uint32_t *)(&pPlayer->fYAcceleration));
	buffer[7] = 1;
	buffer[8] = 4;
	
	for (i = 0; i < 9; i++)
	{
		buffer[i] = htonl(buffer[i]);
	} // for
	
	left = sizeof(uint32_t) * 9;
	pBuffer = &buffer[0];
	while (left > 0)
	{
		sent = sendto(sockfd, pBuffer, left, 0, &dest_addr, addrlen);
		left -= sent;
		pBuffer += sent;
	} // while
} // fnSendGameState

void fnGetServerMsg(map_t *pMap, int sockfd)
{
	
} // fnGetServerMsg
