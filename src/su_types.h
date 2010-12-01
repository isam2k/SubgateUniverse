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

/*	*** STRUCTS N TYPES ***		*/
struct material
{
	char			pName[50];
	float			lShininess;
	float			lAmbient[4];
	float			lDiffuse[4];
	float			lSpecular[4];
	struct material *pNext;
};

typedef struct material material_t;

struct vector
{
	int			iVectI;
	float			fVectC[3];
	struct vector		*pNext;
};

typedef struct vector vector_t;

struct face
{
	vector_t		*pVertice1;
	vector_t		*pVertice2;
	vector_t		*pVertice3;
	vector_t		*pNormal;
	material_t		*pMaterial;
	struct face		*pNext;
};

typedef struct face face_t;

struct model
{
	int			iDispListId;
	int			iModelType;
	vector_t		*pVertices;
	vector_t		*pNormals;
	face_t			*pFaces;
	material_t		*pMaterials;
	struct model		*pNext;
};

typedef struct model model_t;

struct player
{
	uint32_t		iPlayerId;			// players ID
	float			fXPos;				// players x position
	float			fYPos;				// players y position
	float			fRotating;			// player is rotating
	float			fRotation;			// players orientation
	float			fAccelerating;			// player is accelerating
	float			fXAcceleration;			// players x acceleration
	float			fYAcceleration;			// players y acceleration
	uint32_t		iShipType;			// ship type

	struct player		*pNext;				// pointer to next player
};

typedef struct player player_t;

struct object
{
	float			fXPos;				// objects x position
	float			fYPos;				// objects y position
	float			fRotation;			// object is rotating
	int			iObjType;			// object type
	
	struct object		*pNext;				// pointer to next object
};

typedef struct object object_t;

struct map
{
	player_t		*pPlayer;			// local player
	player_t		*pRefPlayer;			// reference player
	player_t		*pOpponents;			// other players
	object_t		*pObjects;			// objects on the map
};

typedef struct map map_t;

struct gamestate
{
	uint32_t		iPlayerId;
	float			fXPos;
	float			fYPos;
	float			fRotating;
	float			fRotation;
	float			fAccelerating;
	float			fXAcceleration;
	float			fYAcceleration;
	uint32_t		iShipType;
	uint32_t		iHeading;
};

typedef struct gamestate gamestate_t;
