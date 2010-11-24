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

#define FLAG_ACK 0x00000001
#define FLAG_REF 0x00000002
#define FLAG_UPD 0x00000004
#define FLAG_INI 0x00000008

/* *** GLOBALS *** */
servstate_t server_state;

/* *** FUNCTION DECLARATIONS *** */
static void fnCleanUp(void);						// does all cleanups at exit

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
		if ((flag = fnEvalGameState(rGs, &server_state)) == 0)	// evaluate recieved gamestate
		{
			if (fnDistGameState(rGs, &server_state))	// distribute recieved gamestate
			{
				fnLogEvent(&server_state, "internal error: failed to send message.");
				continue;
			} // if
		} // if (recieved gamestate needs to be distributed)
		else if (flag == (FLAG_ACK | FLAG_INI))
		{
			fnInitGs(&server_state, &rGs);
			
			if ((nPl = fnMkPlayer(rGs, &sourc_addr, &addr_len)) == NULL)
			{
				fnLogEvent(&server_state, "internal error: failed to make player.");
				flag = FLAG_INI | FLAG_REF;
				if (fnAckGameState(&rGs, flag, &sourc_addr, addr_len, server_state.iSockFd))
				{
					fnLogEvent(&server_state, "internal error: failed to send ack.");
					continue;
				} // if
			} // if
			else
			{
				fnLogEvent(&server_state, "new player entered the game.");
				fnAddPlayer(&server_state, nPl);
			} // else
			if (fnAckGameState(&rGs, flag, &sourc_addr, addr_len, server_state.iSockFd))
			{
				fnLogEvent(&server_state, "internal error: failed to send ack.");
				fnRemPlayer(&server_state, nPl->iPlayerId);
				continue;
			} // if
		} // else (new player has been accepted)
		else if (flag == (FLAG_INI | FLAG_REF))
		{
			fnLogEvent(&server_state, "connection request has been refused.");
			if (fnAckGameState(&rGs, flag, &sourc_addr, addr_len, server_state.iSockFd))
			{
				fnLogEvent(&server_state, "internal error: failed to send ack.");
				continue;
			} // if
		} // else (new player has been refused)
	} // while (infinite server loop)
	
	return 0;
} // main

static void fnCleanUp(void)
{
	player_t *pCurrent;
	
	pCurrent = server_state.pPlayers;
	
	while (pCurrent != NULL)
	{
		server_state.pPlayers = pCurrent->pNext;
		free(pCurrent);
		pCurrent = server_state.pPlayers;
	}
	
	close(server_state.iSockFd);
} // fnCleanUp
