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

/* *** INCLUDES *** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "sud_types.h"
#include "sud_nw_utils.h"
#include "sud_pm_utils.h"

/* *** DEFINES *** */
#define FLAG_ACK 0x20000000
#define FLAG_ENT 0x40000000
#define FLAG_INI 0x80000000

/* *** GLOBALS *** */
servstate_t server_state;

/* *** FUNCTION DECLARATIONS *** */
static void fnCleanUp(void);	// does all cleanups at exit

/* *** MAIN *** */
int main(int argc, char *argv[])
{
	struct sockaddr_storage	sourc_addr;
	socklen_t			addr_len;
	gamestate_t			rGs;
	player_t			*nPl;
	uint32_t			flag;
	
	if (atexit(fnCleanUp) != 0)
		return 1;
	
	if (fnInitServerState(&server_state) < 0)
		return 1;
	
	if (fnInitServer(&server_state, argc, argv) < 0)
		return 1;
	
	while (1)
	{
		if (fnRecvGameState(&rGs, &sourc_addr, &addr_len, server_state.iSockFd))	// recieve gamestate
		{
			fnLogEvent(&server_state, "internal error: failed to recieve message.");
			continue;
		} // if
		
		flag = fnEvalGameState(rGs, &server_state);
		
		if ((flag & FLAG_INI) > 0)
		{
			if ((flag & FLAG_ENT) > 0)
			{
				fnInitGs(&server_state, &rGs);
				
				if ((nPl = fnMkPlayer(rGs, &sourc_addr, &addr_len)) == NULL)
				{
					fnLogEvent(&server_state, "internal error: failed to make player.");
					flag ^= FLAG_ENT;
					fnAnsGameState(&rGs, flag, &sourc_addr, addr_len, server_state.iSockFd);
				} // if
				else
				{
					fnLogEvent(&server_state, "new player entered the game.");
					fnAddPlayer(&server_state, nPl);
					fnDistGameState(rGs, flag, &server_state);
				} // else
			} // if
			else
			{
				fnLogEvent(&server_state, "connection request has been refused.");
				fnAnsGameState(&rGs, flag, &sourc_addr, addr_len, server_state.iSockFd);
			} // else
		} // else (new player)
		else
		{
			fnDistGameState(rGs, flag, &server_state);
		} // else

		fnKillDeconnected(&server_state);
	} // while (infinite server loop)
	
	return 0;
} // main

static void fnCleanUp(void)
{
	player_t *pCurrent;
	for (pCurrent = server_state.pPlayers; pCurrent != NULL; pCurrent = server_state.pPlayers)
	{
		server_state.pPlayers = pCurrent->pNext;
		free(pCurrent);
	} // for
	close(server_state.iSockFd);
} // fnCleanUp
