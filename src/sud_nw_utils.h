/*
 *
 *	File:			sud_nw_utils.h
 *	Author:			Samuel Aeberhard
 *	Description:	sud_nw_utils.h provides basic networking capabilities for a
 *					game server. Such as exchanging player coordinates.
 *
 *					For more detailed information see: sud_nw_utils.c
 *
 */

/*	*** FUNCTION PROTOTYPES ***	*/

void fnShowUsage(char *name, char *msg);
int fnInitServer(servstate_t *server_state, int argc, char *argv[]);
int fnRecvGameState(gamestate_t *rGs, struct sockaddr_storage *sourc_addr, socklen_t *addr_len, int sockfd);
int fnDistGameState(gamestate_t rGs, servstate_t *server_state);
int fnAckGameState(gamestate_t rGs, uint32_t flag, struct sockaddr_storage *sourc_addr, socklen_t addr_len, int sockfd);
player_t *fnMkPlayer(gamestate_t rGs, struct sockaddr_storage *sourc_addr, socklen_t *addr_len);
