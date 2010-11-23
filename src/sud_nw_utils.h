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

void fnShowUsage(char *name, char *msg);	// show usage message
int fnInitServer(servstate_t *server_state, int argc, char *argv[]);	// initialize connection from given arguments
int fnRecvGameState(gamestate_t *rGs, struct sockaddr_storage *sourc_addr, socklen_t *addr_len, int sockfd);	// recieves a gamestate from a socket
int fnDistGameState(gamestate_t rGs, servstate_t *server_state);	// distribute a recieved gamestate to connected clients
int fnAckGameState(gamestate_t *rGs, uint32_t flag, struct sockaddr_storage *sourc_addr, socklen_t addr_len, int sockfd);	// ack a recieved gamestate using a specific flag
player_t *fnMkPlayer(gamestate_t rGs, struct sockaddr_storage *sourc_addr, socklen_t *addr_len);	// make player_t struct from gamestate and connection information
