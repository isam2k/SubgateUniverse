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
void fnShowUsage(char *pname, char *msg);
int fnInitSocket(int argc, char *argv[], struct sockaddr *dest_addr, socklen_t *addrlen);
int fnConnGameServer(player_t *pPlayer, int sockfd, struct sockaddr dest_addr, socklen_t addrlen);
int fnSendGameState(player_t *pPlayer, uint32_t msg, int sockfd, struct sockaddr dest_addr, socklen_t addrlen);
int fnGetUpdates(map_t *pMap, int sockfd, uint32_t *msg);
