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
