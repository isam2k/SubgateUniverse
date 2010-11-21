/*
 *
 *	File:			sud_pm_utils.h
 *	Author:			Samuel Aeberhard
 *	Description:	sud_pm_utils.h contains functions to manage connected clients from a server
 *
 *					For further information see sud_pm_utils.c
 *
 */

/*	*** FUNCTION PROTOTYPES ***	*/

int fnInitServerState(servstate_t *server_state);
int fnLogEvent(servstate_t *server_state, char *msg);
player_t *fnSearchPlayer(servstate_t *server_state, int iPlayerId);
void fnAddPlayer(servstate_t *server_state, player_t *pPlayer);
int fnRemPlayer(servstate_t *server_state, int iPlayerId);
uint32_t fnEvalGameState(gamestate_t rGs, servstate_t *server_state);
