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
int fnInitServerState(servstate_t *server_state);	// initialize servstate_t structure
int fnLogEvent(servstate_t *server_state, char *msg);	// write an event to logfile
player_t *fnSearchPlayer(servstate_t *server_state, int iPlayerId);	// search a player in a dynamically linked list
void fnAddPlayer(servstate_t *server_state, player_t *pPlayer);	// add a player to a dynamically linked list
int fnRemPlayer(servstate_t *server_state, int iPlayerId);	// remove a player from a dynamically linked list
uint32_t fnEvalGameState(gamestate_t rGs, servstate_t *server_state);	// evaluate recieved gamestate and return flags
void fnInitGs(servstate_t *server_state, gamestate_t *rGs);	// Initialize gamestate for reply
void fnKillDeconnected(servstate_t *server_state);	// remove players that likely have deconnected
