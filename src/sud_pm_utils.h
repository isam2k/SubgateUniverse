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

int fnInitServerState(servstate_t *server_state);	// initialize servstate_t structure
int fnLogEvent(servstate_t *server_state, char *msg);	// write an event to logfile
player_t *fnSearchPlayer(servstate_t *server_state, int iPlayerId);	// search a player in a dynamically linked list
void fnAddPlayer(servstate_t *server_state, player_t *pPlayer);	// add a player to a dynamically linked list
int fnRemPlayer(servstate_t *server_state, int iPlayerId);	// remove a player from a dynamically linked list
uint32_t fnEvalGameState(gamestate_t rGs, servstate_t *server_state);	// evaluate recieved gamestate and return flags
void fnInitGs(servstate_t *server_state, gamestate_t *rGs);	// Initialize gamestate for reply
