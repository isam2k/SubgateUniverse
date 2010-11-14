/*
 *
 *	File:			suNetworking.h
 *	Author:			Samuel Aeberhard
 *	Description:	suNetworking.h provides basic networking capabilities for
 *					games. Such as exchanging player coordinates and predicting positions
 *					an case of lags.
 *
 */

/*	*** FUNCTION PROTOTYPES ***	*/

void fnShowUsage(char *pname, char *msg);
int fnInitSocket(int argc, char *argv[], struct sockaddr *dest_addr, socklen_t *addrlen);
void fnSendGameState(player_t *pPlayer, int sockfd, struct sockaddr dest_addr, socklen_t addrlen);
void fnGetServerMsg(map_t *pMap, int sockfd);
