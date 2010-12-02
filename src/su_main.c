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
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include "su_types.h"
#include "su_gr_utils.h"
#include "su_fh_utils.h"
#include "su_nw_utils.h"

/* *** GLOBALS *** */
map_t 		*pMap;
model_t		*pModels;

/* *** FUNCTION DECLARATIONS *** */
static void fnCleanUp(void);

/* *** MAIN *** */
int main(int argc, char *argv[])
{
	SDL_Event	event;
	int		leave, sockfd;
	Uint32		tTickStart, tTickEnd, tDTicks, msg;
	struct sockaddr dest_addr;
	socklen_t	addrlen;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1)
	{
		perror("Unable to initialize SDL");
		return 1;
	}
	
	atexit(fnCleanUp);
	srand(time(NULL));
	
	if ((sockfd = fnInitSocket(argc, argv, &dest_addr, &addrlen)) < 0)
	{
		perror("Unable to establish connection");
		return 1;
	} // if
	
	if ((pMap = fnInitMap()) == NULL)	// initialize map structure
	{
		perror("Unable to initialize map");
		return 1;
	} // if
	
	if (fnConnGameServer(pMap->pPlayer, sockfd, dest_addr, addrlen))
	{
		perror("Error while initializing gamestate");
		exit(1);
	} // if

	SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);	// initialize SDL for use of OpenGL
	
	fnInitOpenGl();	// custom inizialisation routines
	fnReshape();	// set viewport and some transformations
	
	if((pModels = fnGetModels()) == NULL)	// load all models from model directory
		return 1;
	fnBuildDefaultLists(pModels);	// build display lists with default numbering from models
	
	leave = 0;
	while (!leave)	// main game loop
	{
		tTickStart = SDL_GetTicks();

		SDL_PollEvent(&event);

		switch (event.type)
		{
			case SDL_QUIT:
				leave = 1;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						leave = 1;
						break;
					case SDLK_LEFT:
						pMap->pPlayer->fRotating = 1.0f;
						break;
					case SDLK_RIGHT:
						pMap->pPlayer->fRotating = -1.0f;
						break;
					case SDLK_UP:
						pMap->pPlayer->fAccelerating = 1.0f;
						break;
					default:
						break;
				} // switch
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					case SDLK_LEFT:
						pMap->pPlayer->fRotating = 0.0f;
						break;
					case SDLK_RIGHT:
						pMap->pPlayer->fRotating = 0.0f;
						break;
					case SDLK_UP:
						pMap->pPlayer->fAccelerating = 0.0f;
						break;
					default:
						break;
				} // switch
				break;
		} // switch
		
		if (fnRender(pMap))
			return 1;
		
		tTickEnd = SDL_GetTicks();
		tDTicks = tTickEnd - tTickStart;
		fnGameUpdate(pMap, tDTicks);
		
		fnGetUpdates(pMap, sockfd, &msg);
		if (fnDrCheck(pMap->pPlayer, pMap->pRefPlayer, tDTicks))
		{
			fnSendGameState(pMap->pPlayer, msg, sockfd, dest_addr, addrlen);
			pMap->pRefPlayer->fXPos = pMap->pPlayer->fXPos;
			pMap->pRefPlayer->fYPos = pMap->pPlayer->fYPos;
			pMap->pRefPlayer->fRotating = pMap->pPlayer->fRotating;
			pMap->pRefPlayer->fRotation = pMap->pPlayer->fRotation;
			pMap->pRefPlayer->fAccelerating = pMap->pPlayer->fAccelerating;
			pMap->pRefPlayer->fXAcceleration = pMap->pPlayer->fXAcceleration;
			pMap->pRefPlayer->fYAcceleration = pMap->pPlayer->fYAcceleration;
		} // if
	} // while

	return 0;
} // main

static void fnCleanUp(void)
{
	object_t *pObject;
	player_t *pPlayer;
	model_t *pModel;
	material_t *pMaterial;
	vector_t *pVector;
	face_t *pFace;
	
	SDL_Quit();
	
	free(pMap->pPlayer);
	
	for (pPlayer = pMap->pOpponents; pPlayer != NULL; pPlayer = pMap->pOpponents)
	{
		pMap->pOpponents = pPlayer->pNext;
		free(pPlayer);
	} // for
	
	for (pObject = pMap->pObjects; pObject != NULL; pObject = pMap->pObjects)
	{
		pMap->pObjects = pObject->pNext;
		free(pObject);
	} // for
	
	for (pModel = pModels; pModel != NULL; pModel = pModels)
	{
		for (pVector = pModel->pVertices; pVector != NULL; pVector = pModel->pVertices)
		{
			pModel->pVertices = pVector->pNext;
			free(pVector);
		} // for
		
		for (pVector = pModel->pNormals; pVector != NULL; pVector = pModel->pNormals)
		{
			pModel->pNormals = pVector->pNext;
			free(pVector);
		} // for
		
		for (pFace = pModel->pFaces; pFace != NULL; pFace = pModel->pFaces)
		{
			pModel->pFaces = pFace->pNext;
			free(pFace);
		} // for
		
		for (pMaterial = pModel->pMaterials; pMaterial != NULL; pMaterial = pModel->pMaterials)
		{
			pModel->pMaterials = pMaterial->pNext;
			free(pMaterial);
		} // for
		
		pModels = pModel->pNext;
		free(pModel);
	} // for
	
	free(pMap);
} // fnCleanUp
