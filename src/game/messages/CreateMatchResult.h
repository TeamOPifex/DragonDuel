#pragma once 

#include "./OPengine.h"

#include "Game.h"

struct CreateMatchResult {
	ui32 matchIndex;
};

struct JoinMatchMessage {
	ui32 matchIndex;
};

struct JoinMatchResult {
	RakNet::RakNetGUID guid;
	ui32 otherPlayers;
	Player players[1];
};

struct SomeoneJoinedResult {
	RakNet::RakNetGUID guid;
};

struct SomeoneLeftMessage {
	ui32 matchIndex;
	RakNet::RakNetGUID guid;
};