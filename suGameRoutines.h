/*
 *
 *	File:			suGameRoutines.h
 *	Author:			Samuel Aeberhard
 *	Description:	All game relevant functions such as the typical init,
 *					display and update functions can be found inside this header.
 *
 */

/*	*** FUNCTION PROTOTYPES ***	*/

/*	***	RENDERING	*/

void fnInit_OpenGl(void);												// function to initialize OpenGl
void fnRender(map_t *pMap);												// main render function
void fnRenderPlayers(player_t *pPlayer, player_t *pListOfPlayers);		// rendering players to screen
void fnRenderPlayer(player_t *pPlayer, player_t *pCurrent);				// render single player with local player as reference
void fnRenderObjects(player_t *pPlayer, object_t *pListOfObjects);		// render objects
void fnReshape(void);													// reshaping transformation

/*	***	LIST HANDLING	*/

map_t *fnInitMap();														// returns a pointer to an empty allocated map structure
player_t *fnAddOpponent(player_t *pList, player_t *pElement);			// adds new player struct to a dynamic list
object_t *fnAddObject(object_t *pList, object_t *pElement);				// adds new object struct to a dynamic list

/*	*** GAME RELATED FUNCTIONS	*/

void fnGameUpdate(map_t *pMap, Uint32 dTicks);							// updates a game scene according to the time coordinate
void fnMapUpdate(map_t *pMap);
