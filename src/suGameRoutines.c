#include <SDL/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <math.h>
#include "suTypes.h"

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
	pCurrent = pListOfPlayers;

	while (pCurrent != NULL && pPlayer != NULL)
	{
		fnRenderPlayer(pPlayer, pCurrent);
		pCurrent = pCurrent->pNext;
	} // while
} // fnRenderPlayers

void fnRender(map_t *pMap)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	fnRenderPlayer(pMap->pPlayer, pMap->pPlayer);
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
	/* - update player - */
	pMap->pPlayer->fRotation += pMap->pPlayer->fRotate * (360.0f * (float)dTicks / 1000.0f);
	pMap->pPlayer->fXAcceleration += pMap->pPlayer->fAccelerate * (cosf(pMap->pPlayer->fRotation / 180.0f * PI) * (50.0f * (float)dTicks / 1000.0f));
	pMap->pPlayer->fYAcceleration += pMap->pPlayer->fAccelerate * (sinf(pMap->pPlayer->fRotation / 180.0f * PI) * (50.0f * (float)dTicks / 1000.0f));

	pMap->pPlayer->fXPos += pMap->pPlayer->fXAcceleration * (float)dTicks / 1000.0f;
	pMap->pPlayer->fYPos += pMap->pPlayer->fYAcceleration * (float)dTicks / 1000.0f;

	if (pMap->pPlayer->fXAcceleration > 0.0f || pMap->pPlayer->fXAcceleration < 0.0f) pMap->pPlayer->fXAcceleration -= pMap->pPlayer->fXAcceleration * 1.5f * (float)dTicks / 1000.0f;
	if (pMap->pPlayer->fYAcceleration > 0.0f || pMap->pPlayer->fYAcceleration < 0.0f) pMap->pPlayer->fYAcceleration -= pMap->pPlayer->fYAcceleration * 1.5f * (float)dTicks / 1000.0f;
	
	/* - update opponents - */
	
	/* - update objects - */
	
} // fnGameUpdate

void fnMapUpdate(map_t *pMap)
{
	player_t	*pCurOp, *pOpTmp;
	object_t	*pCurOb, *pObTmp;
	float		fDist;
	
	if (pMap != NULL && pMap->pPlayer != NULL)
	{
		/* - remove opponents and objects out of range - */
		pCurOp = pMap->pOpponents;
		
		if (pCurOp != NULL)
		{
			fDist = ((pCurOp->fXPos - pMap->pPlayer->fXPos) * (pCurOp->fXPos - pMap->pPlayer->fXPos)) + ((pCurOp->fYPos - pMap->pPlayer->fYPos) * (pCurOp->fYPos - pMap->pPlayer->fYPos));
			if (fDist > SQ_MAX_RANGE)
			{
				pMap->pOpponents = pCurOp->pNext;
				free(pCurOp);
			} // if
			
			while (pCurOp->pNext != NULL)
			{
				fDist = ((pCurOp->pNext->fXPos - pMap->pPlayer->fXPos) * (pCurOp->pNext->fXPos - pMap->pPlayer->fXPos)) + ((pCurOp->pNext->fYPos - pMap->pPlayer->fYPos) * (pCurOp->pNext->fYPos - pMap->pPlayer->fYPos));
				if (fDist > SQ_MAX_RANGE)
				{
					pOpTmp = pCurOp->pNext;
					pCurOp->pNext = pOpTmp->pNext;
					free(pOpTmp);
				}
				if (pCurOp->pNext != NULL) pCurOp = pCurOp->pNext;
			} // while
		} // if
		
		pCurOb = pMap->pObjects;
		
		if (pCurOb != NULL)
		{
			fDist = ((pCurOb->fXPos - pMap->pPlayer->fXPos) * (pCurOb->fXPos - pMap->pPlayer->fXPos)) + ((pCurOb->fYPos - pMap->pPlayer->fYPos) * (pCurOb->fYPos - pMap->pPlayer->fYPos));
			if (fDist > SQ_MAX_RANGE)
			{
				pMap->pObjects = pCurOb->pNext;
				free(pCurOb);
			} // if
		
			while (pCurOb->pNext != NULL)
			{
				fDist = ((pCurOb->pNext->fXPos - pMap->pPlayer->fXPos) * (pCurOb->pNext->fXPos - pMap->pPlayer->fXPos)) + ((pCurOb->pNext->fYPos - pMap->pPlayer->fYPos) * (pCurOb->pNext->fYPos - pMap->pPlayer->fYPos));
				if (fDist > SQ_MAX_RANGE)
				{
					pObTmp = pCurOb->pNext;
					pCurOb->pNext = pObTmp->pNext;
					free(pObTmp);
				}
				if (pCurOb->pNext != NULL) pCurOb = pCurOb->pNext;
			} // while
		} // if
	} // if
} // fnMapUpdate

map_t *fnInitMap()
{
	map_t *pMap;
	player_t *pPlayer;
	
	if ((pMap = malloc(sizeof(map_t))) == NULL)
		return NULL;
	
	if ((pPlayer = malloc(sizeof(player_t))) == NULL)
		return NULL;
	
	pPlayer->fXPos = 0.0f;
	pPlayer->fYPos = 0.0f;
	pPlayer->fRotate = 0.0f;
	pPlayer->fAccelerate = 0.0f;
	pPlayer->fRotation = 0.0f;
	pPlayer->fXAcceleration = 0.0f;
	pPlayer->fYAcceleration = 0.0f;
	pPlayer->iShipType = 1;
	pPlayer->pNext = NULL;
	
	pMap->pPlayer = pPlayer;
	pMap->pOpponents = NULL;
	pMap->pObjects = NULL;
	
	return pMap;
} // fnInitMap

player_t *fnAddOpponent(player_t *pList, player_t *pElement)
{
	if (pList == NULL)
	{
		pElement->pNext = NULL;
		return pElement;
	}
	else
	{
		if (pList->pNext == NULL)
		{
			pList->pNext = pElement;
			pElement->pNext = NULL;
		}
		else
		{
			fnAddOpponent(pList->pNext, pElement);
		}
	}
	return pList;
} // fnAddOpponent

object_t *fnAddObject(object_t *pList, object_t *pElement)
{
	if (pList == NULL)
	{
		pElement->pNext = NULL;
		return pElement;
	}
	else
	{
		if (pList->pNext == NULL)
		{
			pList->pNext = pElement;
			pElement->pNext = NULL;
		}
		else
		{
			fnAddObject(pList->pNext, pElement);
		}
	}
	return pList;
} // fnAddObject
