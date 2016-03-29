#pragma once

#include "./OPengine.h"
#include "GameMessage.h"

#pragma comment(lib, "Ws2_32.lib")

#include "RakNetTypes.h"

struct GameClient {
	RakNet::RakPeerInterface* client;
	RakNet::Packet* cp;
	ui8 clientPacketIdentifier;
	i8 Connected;

	void(*MessageHandler)(GameMessage, ui8*, ui32);

	i8 Start(ui16 port, ui16 serverPort, const i8* serverIP);
	void Send(GameMessage message);
	void Send(GameMessage message, ui8* data, ui32 length);
	void Update(OPtimer* timer);
	void HandleMessage(ui8* data, ui32 length);
};

extern GameClient CLIENT;