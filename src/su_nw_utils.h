/*
 *
 *	File:		su_nw_utils.h
 *	Author:		Samuel Aeberhard
 *	Description:	su_nw_utils.h provides basic networking capabilities for
 *			games. Such as exchanging player coordinates and predicting positions
 *			an case of lags.
 *
 */

/*	*** FUNCTION PROTOTYPES ***	*/

void fnShowUsage(char *pname, char *msg);
int fnInitSocket(int argc, char *argv[], struct sockaddr *dest_addr, socklen_t *addrlen);
int fnConnGameServer(player_t *pPlayer, int sockfd, struct sockaddr dest_addr, socklen_t addrlen);
int fnSendGameState(player_t *pPlayer, int sockfd, struct sockaddr dest_addr, socklen_t addrlen);
int fnGetUpdates(map_t *pMap, int sockfd);
