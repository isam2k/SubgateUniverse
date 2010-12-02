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

#include <time.h>

#include <arpa/inet.h>

#include "sud_types.h"

/* *** DEFINES *** */
#define FLAG_ACK 0x20000000
#define FLAG_ENT 0x40000000
#define FLAG_INI 0x80000000
#define IDLE_TIMEOUT 300

int fnInitServerState(servstate_t *server_state)
{
	player_t	*pPlayerRoot;
	
	memset(server_state, 0, sizeof(servstate_t));
	
	if((pPlayerRoot = malloc(sizeof(player_t))) == NULL)
		return 1;
	
	memset(pPlayerRoot, 0, sizeof(player_t));
	
	server_state->iCurPlayerId = 1;
	server_state->pPlayers = pPlayerRoot;
	return 0;
} // fnInitServerState

int fnLogEvent(servstate_t *server_state, char *msg)
{
	FILE *fd;
	size_t wrote;
	time_t now;
	struct tm *ts;
	char buffer[30];
	
	now = time(NULL);
	ts = localtime(&now);
	
	wrote = strftime(buffer, sizeof(buffer), "%d-%m-%y %H:%M:%S : ", ts);
	
	if ((fd = fopen(server_state->LogFile, "a")) == NULL)
		return 1;
	
	fwrite(buffer, 1, wrote, fd);
	fwrite(msg, 1, strlen(msg), fd);
	fwrite("\n", 1, 1, fd);
	
	fclose(fd);
	return 0;
} // fnLogEvent

player_t *fnSearchPlayer(servstate_t *server_state, int iPlayerId)
{
	player_t *pFound, *pCurrent;
	
	if (server_state->pPlayers == NULL || iPlayerId == 0)
		return NULL;
	
	pFound = NULL;
	for (pCurrent = server_state->pPlayers; pCurrent != NULL; pCurrent = pCurrent->pNext)
	{
		if (pCurrent->iPlayerId == iPlayerId)
		{
			pFound = pCurrent;
			break;
		}
	} // for
	
	return pFound;
} // fnSearchPlayer

void fnAddPlayer(servstate_t *server_state, player_t *pPlayer)
{
	pPlayer->pNext = server_state->pPlayers->pNext;
	pPlayer->pPrev = server_state->pPlayers;
	if (pPlayer->pNext != NULL) pPlayer->pNext->pPrev = pPlayer;
	pPlayer->pPrev->pNext = pPlayer;
	server_state->iConnPlayers++;
} // fnAddPlayer

int fnRemPlayer(servstate_t *server_state, int iPlayerId)
{
	player_t *pCurrent;
	
	if ((pCurrent = fnSearchPlayer(server_state, iPlayerId)) == NULL)
		return 1;
	else
	{
		pCurrent->pPrev->pNext = pCurrent->pNext;
		pCurrent->pNext->pPrev = pCurrent->pPrev;
		free(pCurrent);
		server_state->iConnPlayers--;
	} // else
	return 0;
} // fnRemPlayer

uint32_t fnEvalGameState(gamestate_t rGs, servstate_t *server_state)
{
	player_t *pPlayer;
	
	if ((rGs.iHeading & FLAG_ACK) > 0)
	{
		pPlayer = fnSearchPlayer(server_state, rGs.iPlayerId);
		pPlayer->tLastContact = 0;
		return rGs.iHeading ^ FLAG_ACK;
	} // if
	else if ((rGs.iHeading & FLAG_INI) > 0)	// new player sent FLAG_INI
	{
		if (server_state->iConnPlayers < server_state->iMaxPlayers)
		{
			return rGs.iHeading | FLAG_ENT;
		} // if
		else	// server full -> refuse connection
		{
			return rGs.iHeading;
		} // else
	} // else
	
	return 0;
} // fnEvalGameState

void fnInitGs(servstate_t *server_state, gamestate_t *rGs)
{
	rGs->iPlayerId = server_state->iCurPlayerId;
	rGs->fXPos = 0.0f;
	rGs->fYPos = 0.0f;
	rGs->fRotating = 0.0f;
	rGs->fRotation = 0.0f;
	rGs->fAccelerating = 0.0f;
	rGs->fXAcceleration = 0.0f;
	rGs->fYAcceleration = 0.0f;
	server_state->iCurPlayerId++;
} // fnInitGs

void fnKillDeconnected(servstate_t *server_state)
{
	time_t tNow;
	player_t *pCurrent;
	
	tNow = time(NULL);
	
	for (pCurrent = server_state->pPlayers->pNext; pCurrent != NULL; pCurrent = pCurrent->pNext)
	{
		if (pCurrent->tLastContact < tNow - IDLE_TIMEOUT && pCurrent->tLastContact != 0)
		{
			if (pCurrent->pNext == NULL)
				pCurrent->pPrev->pNext = NULL;
			else
			{
				pCurrent->pPrev->pNext = pCurrent->pNext;
				pCurrent->pNext->pPrev = pCurrent->pPrev;
			} // else
			fnLogEvent(server_state, "A player has left the game.");
			free(pCurrent);
			break;
		} // if
	} // for
} // fnKillDeconnected
