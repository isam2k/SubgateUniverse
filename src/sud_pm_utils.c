/* *** INCLUDES *** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include "sud_types.h"

/* *** DEFINES *** */

#define FLAG_ACK 0x00000001
#define FLAG_REF 0x00000002
#define FLAG_UPD 0x00000004
#define FLAG_INI 0x00000008

int fnInitServerState(servstate_t *server_state)
{
	player_t	*pPlayerRoot;
	packet_t	*pPacketRoot;
	
	memset(server_state, 0, sizeof(servstate_t));
	
	if((pPlayerRoot = malloc(sizeof(player_t))) == NULL)
		return 1;
	if((pPacketRoot = malloc(sizeof(packet_t))) == NULL)
	{
		free(pPlayerRoot);
		return 1;
	} // if
	
	pPlayerRoot->iPlayerId = 0;
	pPlayerRoot->pPrev = NULL;
	pPlayerRoot->pNext = NULL;
	
	pPacketRoot->timestamp = 0;
	pPacketRoot->pGs = NULL;
	pPacketRoot->pNext = NULL;
	
	server_state->iCurPlayerId = 1;
	server_state->pPlayers = pPlayerRoot;
	server_state->pPackets = pPacketRoot;
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
	if ((rGs.iHeading & FLAG_ACK) > 0)
	{
		return -1;
	} // if
	else if ((rGs.iHeading & FLAG_INI) > 0)		// new player sent FLAG_INI
	{
		if (server_state->iConnPlayers < server_state->iMaxPlayers)
		{
			return FLAG_INI | FLAG_ACK;
		} // if
		else		// server full -> refuse connection
		{
			return FLAG_INI | FLAG_REF;
		} // else
	} // else
	else
	{
		return 0;
	} // else
} // fnEvalGameState

void fnInitGs(servstate_t *server_state, gamestate_t *rGs)
{
	rGs->iPlayerId = server_state->iCurPlayerId;
	rGs->fXPos = 0.0f;
	rGs->fYPos = 0.0f;
	rGs->fRotating = 0.0f;
	rGs->fRotation = 0.0f;
	rGs->fXAccel = 0.0f;
	rGs->fYAccel = 0.0f;
	rGs->iShipType = 0;
	server_state->iCurPlayerId++;
} // fnInitGs
