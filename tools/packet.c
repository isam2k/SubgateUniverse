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

/* *** DEFINES *** */

#define DEF_PORTNUM "6249"
#define DEF_MAXCONN 4
#define FLAG_ACK 0x00000001
#define FLAG_REF 0x00000002
#define FLAG_MSG 0x00000004
#define FLAG_INI 0x00000008

struct gamestate
{
	uint32_t		iPlayerId;
	float			fXPos;
	float			fYPos;
	float			fRotating;
	float			fRotation;
	float			fXAccel;
	float			fYAccel;
	uint32_t		iShipType;
	uint32_t		iHeading;
};

typedef struct gamestate gamestate_t;

int fnInitSocket(struct sockaddr *dest_addr, socklen_t *addrlen);
int fnRecvGameState(int sockfd);
int fnSendGameState(int iPlayerId, int iFlag, int sockfd, struct sockaddr dest_addr, socklen_t addrlen);

int main(int argc, char *argv[])
{
	struct sockaddr dest_addr;
	socklen_t addr_len;
	int sockfd, opt;
	uint32_t iFlag, iPlayerId;
	
	if ((sockfd = fnInitSocket(&dest_addr, &addr_len)) < 0)
		return 1;
	
	system("clear");
	printf("SUD TEST CLIENT\n");
	printf("This tool can be used to test the functionality of the sud application.\n");
	printf("For more information consult the readme.\n");
	
	iFlag = 0;
	iPlayerId = 0;
	while (1)
	{
		printf("Compose the message you want to send:\n");
		printf("\t1.\tSet FLAG_MSG\n");
		printf("\t2.\tSet FLAG_INI\n");
		printf("\t3.\tSet FLAG_ACK\n");
		printf("\t4.\tSet FLAG_REF\n");
		printf("\t5.\tReset flag\n");
		printf("\t6.\tSet iPlayerId\n");
		printf("\t7.\tSend Packet\n");
		printf("\t8.\tExit\n");
		
		printf("\n\tiFlag: %d\n", iFlag);
		printf("\tiPlayerId: %d\n", iPlayerId);
		
		printf("user:~choice$ "); scanf("%d", &opt);
		
		switch (opt)
		{
			case 1	:	iFlag = iFlag | FLAG_MSG;
						break;
			case 2	:	iFlag = iFlag | FLAG_INI;
						break;
			case 3	:	iFlag = iFlag | FLAG_ACK;
						break;
			case 4	:	iFlag = iFlag | FLAG_REF;
						break;
			case 5	:	iFlag = 0;
						break;
			case 6	:	printf("user:~iPlayerId$ "); scanf("%d", &iPlayerId);
						break;
			case 7	:	if (fnSendGameState(iPlayerId, iFlag, sockfd, dest_addr, addr_len))
						{
							perror("Unable to send packet");
							break;
						} // if
						iFlag = 0;
						iPlayerId = 0;
						if (fnRecvGameState(sockfd))
						{
							perror("Unable to retrieve packet");
						} // if
						break;
			case 8	:	close(sockfd);
						return 0;
			default:	printf("Invalid choice.\n");
						break;
		} // switch
	} // while
	
	return 0;
} // main

int fnSendGameState(int iPlayerId, int iFlag, int sockfd, struct sockaddr dest_addr, socklen_t addrlen)
{
	ssize_t sent;
	size_t left;
	uint32_t buffer[9], *pBuffer;
	
	buffer[0] = htonl(iPlayerId);
	buffer[8] = htonl(iFlag);
	
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

int fnRecvGameState(int sockfd)
{
	int i;
	size_t left;
	ssize_t read;
	uint32_t buffer[9], *pBuffer;
	struct sockaddr sourc_addr;
	socklen_t addr_len;
	
	addr_len = sizeof(struct sockaddr);
	
	left = sizeof(uint32_t) * 9;
	pBuffer = &buffer[0];
	while (left > 0)
	{
		if ((read = recvfrom(sockfd, pBuffer, left, 0, &sourc_addr, &addr_len)) < 0)
			return 1;
		left -= read;
		pBuffer += read;
	} // while
	
	for (i = 0; i < 9; i++)
	{
		buffer[i] = ntohl(buffer[i]);
	} // for
	printf("\t~ANSWER~\n");
	printf("iPlayerId: %x\n", buffer[0]);
	printf("iFlag: %x\n", buffer[8]);
	return 0;
} // fnRecvGameState

int fnInitSocket(struct sockaddr *dest_addr, socklen_t *addrlen)
{
	int					sockfd, state;
	struct addrinfo		hints, *serv_addr;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	
	if ((state = getaddrinfo("localhost", "6249", &hints, &serv_addr)) != 0)
		return -1;
	
	if ((sockfd = socket(serv_addr->ai_family, serv_addr->ai_socktype, serv_addr->ai_protocol)) < 0)
		return -1;
	
	*dest_addr = *((struct sockaddr *)serv_addr->ai_addr);
	*addrlen = serv_addr->ai_addrlen;
	
	freeaddrinfo(serv_addr);
	return sockfd;
} // fnInitSocket
