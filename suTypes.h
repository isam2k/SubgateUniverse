/*	*** STRUCTS N TYPES ***		*/

struct material
{
	char			pName[50];
	GLfloat			lShininess;
	GLfloat			lAmbient[4];
	GLfloat			lDiffuse[4];
	GLfloat			lSpecular[4];
	struct material *pNext;
};

typedef struct material material_t;

struct vector
{
	GLint			iVectI;
	GLfloat			fVectC[3];
	struct vector	*pNext;
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
	GLint			iDispListId;
	GLint			iModelType;
	face_t			*pFaces;
	material_t		*pMaterials;
	struct model	*pNext;
};

typedef struct model model_t;

struct player
{
	GLfloat			fXPos;					// players x position
	GLfloat			fYPos;					// players y position
	GLfloat			fRotate;				// player is rotating
	GLfloat			fAccelerate;			// player is accelerating
	GLfloat			fRotation;				// players orientation
	GLfloat			fXAcceleration;			// players x acceleration
	GLfloat			fYAcceleration;			// players y acceleration
	int				iShipType;				// ship type

	struct player	*pNext;				// pointer to next player
};

typedef struct player player_t;

struct object
{
	GLfloat			fXPos;					// objects x position
	GLfloat			fYPos;					// objects y position
	GLfloat			fRotate;				// object is rotating
	int				iObjType;				// object type
	
	struct object	*pNext;					// pointer to next object
};

typedef struct object object_t;

struct map
{
	player_t		*pPlayer;				// local player
	player_t		*pOpponents;			// other players
	object_t		*pObjects;				// objects on the map
};

typedef struct map map_t;

struct gamestate
{
	uint32_t		iPlayerId;
	float			fXPos;
	float			fYPos;
	float			fRotating;
	float			fRotation;
	float			fXAccel;
	float			fYAccel;
	uint32_t		iHeading;
};

typedef struct gamestate gamestate_t;
