/*
 *
 *	File:			su_main.c
 *	Author:			Samuel Aeberhard
 *	Description:	su_main.c contains the main function for the game,
 *					it also initializes all components and contains the main game loop.
 *					It also makes time measuring for a time-based rendering.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
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

int main(int argc, char *argv[])
{
	SDL_Event	event;
	model_t		*pModels;
	map_t		*pMap;
	int			leave, sockfd;
	Uint32		tTickStart, tTickEnd, tDTicks;
	struct sockaddr dest_addr;
	socklen_t	addrlen;

	if (SDL_Init(SDL_INIT_TIMER) == -1)
	{
		perror("Unable to initialize SDL");
		exit(1);
	}

	atexit(SDL_Quit);
	srand(time(NULL));
	
	if ((sockfd = fnInitSocket(argc, argv, &dest_addr, &addrlen)) < 0)
	{
		perror("Unable to establish connection");
		exit(1);
	} // if
	
	pMap = fnInitMap();							// initialize map structure
	
	if (fnConnGameServer(pMap->pPlayer, sockfd, dest_addr, addrlen))
	{
		perror("Error while initializing gamestate");
		exit(1);
	} // if

	SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);		// initialize SDL for use of OpenGL
	
	fnInitOpenGl();								// custom inizialisation routines
	fnReshape();								// set viewport and some transformations
	
	pModels = fnGetModels();					// load all models from model directory
	fnBuildDefaultLists(pModels);				// build display lists with default numbering from models
	
	leave = 0;
	while (!leave)			// main game loop
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
						pMap->pPlayer->fRotate = 1.0f;
						break;
					case SDLK_RIGHT:
						pMap->pPlayer->fRotate = -1.0f;
						break;
					case SDLK_UP:
						pMap->pPlayer->fAccelerate = 1.0f;
						break;
					default:
						break;
				} // switch
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					case SDLK_LEFT:
						pMap->pPlayer->fRotate = 0.0f;
						break;
					case SDLK_RIGHT:
						pMap->pPlayer->fRotate = 0.0f;
						break;
					case SDLK_UP:
						pMap->pPlayer->fAccelerate = 0.0f;
						break;
					default:
						break;
				} // switch
				break;
		} // switch
		
		fnRender(pMap);

		tTickEnd = SDL_GetTicks();
		tDTicks = tTickEnd - tTickStart;
		
		fnGameUpdate(pMap, tDTicks);
		fnMapUpdate(pMap);
		fnSendGameState(pMap->pPlayer, sockfd, dest_addr, addrlen);
		fnGetServerMsg(pMap, sockfd);
	} // while

	return 0;
} // main
