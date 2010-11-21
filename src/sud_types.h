struct gamestate
{
	uint32_t		iPlayerId;
	float			fXPos;
	float			fYPos;
	float			fRotating;
	float			fRotation;
	float			fXAccel;
	float			fYAccel;
	uint32_t		iShipType;
	uint32_t		iHeading;
};

typedef struct gamestate gamestate_t;

struct packet
{
	time_t			timestamp;
	gamestate_t		*pGs;
	struct packet	*pNext;
};

typedef struct packet packet_t;

struct player
{
	uint32_t		iPlayerId;				// players id
	float			fXPos;					// players x position
	float			fYPos;					// players y position
	float			fRotating;				// player is rotating
	float			fRotation;				// players orientation
	float			fXAccel;				// players x acceleration
	float			fYAccel;				// players y acceleration
	uint32_t		iShipType;				// ship type
	
	struct sockaddr_storage addr;			// players address
	socklen_t addrlen;						// address length
	struct player	*pPrev;					// pointer to previous player
	struct player	*pNext;					// pointer to next player
};

typedef struct player player_t;

struct servstate
{
	int			iConnPlayers;
	int			iMaxPlayers;
	int			iSockFd;
	char		LogFile[20];
	player_t	*pPlayers;
	packet_t	*pPackets;
};

typedef struct servstate servstate_t;
