#include <SDL/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <math.h>
#include <arpa/inet.h>
#include "su_types.h"

/*	*** DEFINES ***		*/

#define PI 3.151492f
#define WND_WIDTH 800
#define WND_HEIGHT 600
#define SQ_MAX_RANGE 200.0f

/*	*** FUNCTION DECLARATIONS ***	*/

void fnInitOpenGl(void)
{
	GLfloat fPosition0[] = {1.0f, 1.0f, 1.0f, 0.0f};
	GLfloat fAmbientLight0[] = {0.4f, 0.4f, 0.4f, 1.0f};
	GLfloat fDiffuseLight0[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat fSpecularLight0[] = {1.0f, 1.0f, 1.0f, 1.0f};
	
	GLfloat fGlobalAmbient[] = {0.7f,0.7f, 0.7f, 1.0f};
	
	GLfloat fFogColor[] = {0.0f, 0.0f, 0.0f, 1.0f};

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);

	glLightfv(GL_LIGHT0, GL_POSITION, fPosition0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, fAmbientLight0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fDiffuseLight0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fSpecularLight0);
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fGlobalAmbient);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

	glShadeModel(GL_FLAT);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogfv(GL_FOG_COLOR, fFogColor);
	glFogf(GL_FOG_DENSITY, 0.0f);
	glHint(GL_FOG_HINT, GL_NICEST);
} // fnInitOpenGl

void fnRenderPlayer(player_t *pPlayer, player_t *pCurrent)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(pCurrent->fXPos - pPlayer->fXPos, pCurrent->fYPos - pPlayer->fYPos, -10.0f);
	glRotatef(pCurrent->fRotation, 0.0f, 0.0f, 1.0f);
	glScalef(1.0f, 1.0f, 1.0f);
	
	glStencilFunc(GL_ALWAYS, 1, 0xFFFF);
	glStencilOp(GL_KEEP,GL_KEEP, GL_REPLACE);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(pCurrent->iShipType);
	
	glDisable(GL_LIGHTING);
	
	glStencilFunc(GL_NOTEQUAL, 1, 0xFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.0f);
	
	glPushMatrix();
		glColor3f(0.0f, 0.0f, 0.0f);
		glCallList(pCurrent->iShipType);
	glPopMatrix();
	
	glEnable(GL_LIGHTING);
} // fnRenderPlayer

void fnRenderObjects(player_t *pPlayer, object_t *pListOfObjects)
{
	if (pListOfObjects != NULL && pPlayer != NULL)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(pListOfObjects->fXPos - pPlayer->fXPos, pListOfObjects->fYPos - pPlayer->fYPos, -10.0f);
		glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
		glScalef(1.0f, 1.0f, 1.0f);
		
		glStencilFunc(GL_ALWAYS, 1, 0xFFFF);
		glStencilOp(GL_KEEP,GL_KEEP, GL_REPLACE);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glCallList(pListOfObjects->iObjType);
		
		glDisable(GL_LIGHTING);
		
		glStencilFunc(GL_NOTEQUAL, 1, 0xFFFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(3.0f);
		
		glPushMatrix();
			glColor3f(0.0f, 0.0f, 0.0f);
			glCallList(pListOfObjects->iObjType);
		glPopMatrix();
		
		glEnable(GL_LIGHTING);
		
		fnRenderObjects(pPlayer, pListOfObjects->pNext);
	}
} // fnRenderObjects

void fnRenderPlayers(player_t *pPlayer, player_t *pListOfPlayers)
{
	player_t *pCurrent;

	for (pCurrent = pListOfPlayers; pCurrent != NULL && pPlayer != NULL; pCurrent = pCurrent->pNext)
	{
		fnRenderPlayer(pPlayer, pCurrent);
	} // for
} // fnRenderPlayers

void fnRender(map_t *pMap)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	fnRenderPlayer(pMap->pPlayer, pMap->pPlayer);
	// fnRenderPlayer(pMap->pPlayer, pMap->pRefPlayer);
	fnRenderPlayers(pMap->pPlayer, pMap->pOpponents);
	fnRenderObjects(pMap->pPlayer, pMap->pObjects);

	SDL_GL_SwapBuffers();
} // fnRender

void fnReshape(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 60.0f);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, WND_WIDTH, WND_HEIGHT);
} // fnReshape

void fnGameUpdate(map_t *pMap, Uint32 dTicks)
{
	player_t *pCurrent;
	/* - update player - */
	pMap->pPlayer->fRotation += pMap->pPlayer->fRotating * (360.0f * (float)dTicks / 1000.0f);
	pMap->pPlayer->fXAcceleration += pMap->pPlayer->fAccelerating * (cosf(pMap->pPlayer->fRotation / 180.0f * PI) * (50.0f * (float)dTicks / 1000.0f));
	pMap->pPlayer->fYAcceleration += pMap->pPlayer->fAccelerating * (sinf(pMap->pPlayer->fRotation / 180.0f * PI) * (50.0f * (float)dTicks / 1000.0f));

	pMap->pPlayer->fXPos += pMap->pPlayer->fXAcceleration * (float)dTicks / 1000.0f;
	pMap->pPlayer->fYPos += pMap->pPlayer->fYAcceleration * (float)dTicks / 1000.0f;

	if (pMap->pPlayer->fXAcceleration != 0.0f) pMap->pPlayer->fXAcceleration -= pMap->pPlayer->fXAcceleration * 1.5f * (float)dTicks / 1000.0f;
	if (pMap->pPlayer->fYAcceleration != 0.0f) pMap->pPlayer->fYAcceleration -= pMap->pPlayer->fYAcceleration * 1.5f * (float)dTicks / 1000.0f;
	
	/* - update opponents by applying the dr predictive algorithm - */
	for	(pCurrent = pMap->pOpponents; pCurrent != NULL; pCurrent = pCurrent->pNext)
	{
		if (pCurrent->fRotation != 0)
		{
			pCurrent->fRotation += pCurrent->fRotating * (360.0f * (float)dTicks / 1000.0f);
		} // if
		
		if (pCurrent->fAccelerating != 0)
		{
			pCurrent->fXAcceleration += pCurrent->fAccelerating * (cosf(pCurrent->fRotation / 180.0f * PI) * (50.0f * (float)dTicks / 1000.0f));
			pCurrent->fYAcceleration += pCurrent->fAccelerating * (sinf(pCurrent->fRotation / 180.0f * PI) * (50.0f * (float)dTicks / 1000.0f));
		} // if
		
		if (pCurrent->fXAcceleration != 0 || pCurrent->fYAcceleration != 0)
		{
			pCurrent->fXPos += pCurrent->fXAcceleration * (float)dTicks / 1000.0f;
			pCurrent->fYPos += pCurrent->fYAcceleration * (float)dTicks / 1000.0f;
			pCurrent->fXAcceleration -= pCurrent->fXAcceleration * 1.5f * (float)dTicks / 1000.0f;
			pCurrent->fYAcceleration -= pCurrent->fYAcceleration * 1.5f * (float)dTicks / 1000.0f;
		} // if
	} // for
} // fnGameUpdate

map_t *fnInitMap()
{
	map_t *pMap;
	player_t *pPlayer, *pRefPlayer;
	
	if ((pMap = malloc(sizeof(map_t))) == NULL)
		return NULL;
	
	if ((pPlayer = malloc(sizeof(player_t))) == NULL)
	{
		free(pMap);
		return NULL;
	} // if
	
	if ((pRefPlayer = malloc(sizeof(player_t))) == NULL)
	{
		free(pMap);
		free(pPlayer);
		return NULL;
	} // if
	
	pPlayer->fXPos = 0.0f;
	pPlayer->fYPos = 0.0f;
	pPlayer->fRotating = 0.0f;
	pPlayer->fRotation = 0.0f;
	pPlayer->fAccelerating = 0.0f;
	pPlayer->fXAcceleration = 0.0f;
	pPlayer->fYAcceleration = 0.0f;
	pPlayer->iShipType = 1;
	pPlayer->pNext = NULL;
	
	pRefPlayer->fXPos = 0.0f;
	pRefPlayer->fYPos = 0.0f;
	pRefPlayer->fRotating = 0.0f;
	pRefPlayer->fRotation = 0.0f;
	pRefPlayer->fAccelerating = 0.0f;
	pRefPlayer->fXAcceleration = 0.0f;
	pRefPlayer->fYAcceleration = 0.0f;
	pRefPlayer->iShipType = 1;
	pRefPlayer->pNext = NULL;
	
	pMap->pPlayer = pPlayer;
	pMap->pRefPlayer = pRefPlayer;
	pMap->pOpponents = NULL;
	pMap->pObjects = NULL;
	
	return pMap;
} // fnInitMap

int fnDrCheck(player_t *pPlayer, player_t *pRefPlayer, Uint32 dTicks)
{
	if (pRefPlayer->fRotation != 0)
	{
		pRefPlayer->fRotation += pRefPlayer->fRotating * (360.0f * (float)dTicks / 1000.0f);
	} // if
	
	if (pRefPlayer->fAccelerating != 0)
	{
		pRefPlayer->fXAcceleration += pRefPlayer->fAccelerating * (cosf(pRefPlayer->fRotation / 180.0f * PI) * (50.0f * (float)dTicks / 1000.0f));
		pRefPlayer->fYAcceleration += pRefPlayer->fAccelerating * (sinf(pRefPlayer->fRotation / 180.0f * PI) * (50.0f * (float)dTicks / 1000.0f));
	} // if
	
	if (pRefPlayer->fXAcceleration != 0 || pRefPlayer->fYAcceleration != 0)
	{
		pRefPlayer->fXPos += pRefPlayer->fXAcceleration * (float)dTicks / 1000.0f;
		pRefPlayer->fYPos += pRefPlayer->fYAcceleration * (float)dTicks / 1000.0f;
		pRefPlayer->fXAcceleration -= pRefPlayer->fXAcceleration * 1.5f * (float)dTicks / 1000.0f;
		pRefPlayer->fYAcceleration -= pRefPlayer->fYAcceleration * 1.5f * (float)dTicks / 1000.0f;
	} // if
	
	if (pPlayer->fRotation > pRefPlayer->fRotation + 5.0f
		|| pPlayer->fRotation < pRefPlayer->fRotation - 5.0f)
		return 1;
	if (pPlayer->fXPos > pRefPlayer->fXPos + 0.1f
		|| pPlayer->fXPos < pRefPlayer->fXPos - 0.1f
		|| pPlayer->fYPos > pRefPlayer->fYPos + 0.1f
		|| pPlayer->fYPos < pRefPlayer->fYPos - 0.1f)
		return 1;
	return 0;
} // if
