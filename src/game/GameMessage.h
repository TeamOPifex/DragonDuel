#pragma once

#include "MessageIdentifiers.h"

enum GameMessage {
	GENERIC = ID_USER_PACKET_ENUM,
	ClientConnected,
	BackgroundColor,
	PositionUpdate,
	CreateMatch,
	JoinMatch,
	SomeoneJoined,
	Ready,
	SomeoneLeft
};