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

struct player
{
	uint32_t		iPlayerId;		// players id
	float			fXPos;			// players x position
	float			fYPos;			// players y position
	float			fRotating;		// player is rotating
	float			fRotation;		// players orientation
	float			fAccelerating;		// player is accelerating
	float			fXAcceleration;		// players x acceleration
	float			fYAcceleration;		// players y acceleration
	uint32_t		iShipType;		// ship type
	
	time_t			tLastContact;		// timestamp of the last unacked sent packet
	struct sockaddr_storage addr;			// players address
	socklen_t		addrlen;		// address length
	struct player		*pPrev;			// pointer to previous player
	struct player		*pNext;			// pointer to next player
};

typedef struct player player_t;

struct servstate
{
	int			iConnPlayers;
	int			iMaxPlayers;
	int			iCurPlayerId;
	int			iSockFd;
	char			LogFile[20];
	player_t		*pPlayers;
};

typedef struct servstate servstate_t;
