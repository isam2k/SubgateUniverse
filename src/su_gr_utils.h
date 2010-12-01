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

/*	***	RENDERING	*/
void fnInitOpenGl(void);	// function to initialize OpenGl
int fnRender(map_t *pMap);	// main render function
int fnRenderPlayer(map_t *pMap);	// render single player
int fnRenderObjects(map_t *pMap);	// render objects
int fnRenderPlayers(map_t *pMap);	// rendering players to screen
void fnReshape(void);	// reshaping transformation

/*	***	LIST HANDLING	*/
map_t *fnInitMap();	// returns a pointer to an empty allocated map structure

/*	*** GAME RELATED FUNCTIONS	*/
void fnGameUpdate(map_t *pMap, Uint32 dTicks);	// updates a game scene according to the time coordinate
int fnDrCheck(player_t *pPlayer, player_t *pRefPlayer, Uint32 dTicks);	// applies dead reckoning algorithm on player
