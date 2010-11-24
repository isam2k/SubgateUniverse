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
	Uint32		tTickStart, tTickEnd, tDTicks;
	struct sockaddr dest_addr;
	socklen_t	addrlen;

	if (SDL_Init(SDL_INIT_TIMER) == -1)
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
	
	pModels = fnGetModels();	// load all models from model directory
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
		
		if (fnDrCheck(pMap->pPlayer, pMap->pRefPlayer, tDTicks))
		{
			fnSendGameState(pMap->pPlayer, sockfd, dest_addr, addrlen);
			*(pMap->pRefPlayer) = *(pMap->pPlayer);
		} // if
		
		fnGetUpdates(pMap, sockfd);
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
