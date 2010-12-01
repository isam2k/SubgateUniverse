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

/*	*** FUNCTION PROTOTYPES ***	*/
void fnShowUsage(char *name, char *msg);	// show usage message
int fnInitServer(servstate_t *server_state, int argc, char *argv[]);	// initialize connection from given arguments
int fnRecvGameState(gamestate_t *rGs, struct sockaddr_storage *sourc_addr, socklen_t *addr_len, int sockfd);	// recieves a gamestate from a socket
int fnDistGameState(gamestate_t rGs, uint32_t flag, servstate_t *server_state);	// distribute a recieved gamestate to connected clients
int fnAnsGameState(gamestate_t *rGs, uint32_t flag, struct sockaddr_storage *sourc_addr, socklen_t addr_len, int sockfd);	// ack a recieved gamestate using a specific flag
player_t *fnMkPlayer(gamestate_t rGs, struct sockaddr_storage *sourc_addr, socklen_t *addr_len);	// make player_t struct from gamestate and connection information
