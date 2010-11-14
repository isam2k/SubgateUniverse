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
#include "sudTypes.h"
#include "sudNetworking.h"
#include "sudRoutines.h"

int main(int argc, char *argv[])
{
	int 						sockfd, nsockfd, i;
	uint32_t					buffer[8], *pBuffer;
	struct sockaddr_storage		their_addr;
	socklen_t					addr_len;
	ssize_t						got, left;
	size_t						total;
	gamestate_t					rcvdGs;
	
	sockfd = fnInitSocket(argc, argv);
	
	while (1)
	{
		total = sizeof(uint32_t) * 8;
		got = 0;
		pBuffer = &buffer[0];
		while (got < total)
		{
			left = total - got;
			got += recvfrom(sockfd, pBuffer + got, left, 0, (struct sockaddr *)&their_addr, &addr_len);
		} // while
		
		for (i = 0; i < 8; i++)
		{
			buffer[i] = ntohl(buffer[i]);
		} // for
		
		rcvdGs.iPlayerId = buffer[0];
		rcvdGs.fXPos = *((float *)(&buffer[1]));
		rcvdGs.fYPos = *((float *)(&buffer[2]));
		rcvdGs.fRotating = *((float *)(&buffer[3]));
		rcvdGs.fRotation = *((float *)(&buffer[4]));
		rcvdGs.fXAccel = *((float *)(&buffer[5]));
		rcvdGs.fYAccel = *((float *)(&buffer[6]));
		rcvdGs.iHeading = buffer[7];
	} // while
	
	return 0;
}
