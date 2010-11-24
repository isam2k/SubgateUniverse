/*
 *
 *	File:			su_gr_utils.h
 *	Author:			Samuel Aeberhard
 *	Description:	All game relevant functions such as the typical init,
 *					display and update functions can be found inside this header.
 *
 */

/*	*** FUNCTION PROTOTYPES ***	*/

/*	***	RENDERING	*/

void fnInitOpenGl(void);	// function to initialize OpenGl
void fnRender(map_t *pMap);	// main render function
void fnRenderPlayers(player_t *pPlayer, player_t *pListOfPlayers);	// rendering players to screen
void fnRenderPlayer(player_t *pPlayer, player_t *pCurrent);	// render single player with local player as reference
void fnRenderObjects(player_t *pPlayer, object_t *pListOfObjects);	// render objects
void fnReshape(void);	// reshaping transformation

/*	***	LIST HANDLING	*/

map_t *fnInitMap();	// returns a pointer to an empty allocated map structure

/*	*** GAME RELATED FUNCTIONS	*/

void fnGameUpdate(map_t *pMap, Uint32 dTicks);	// updates a game scene according to the time coordinate
int fnDrCheck(player_t *pPlayer, player_t *pRefPlayer, Uint32 dTicks);	// applies dead reckoning algorithm on player
