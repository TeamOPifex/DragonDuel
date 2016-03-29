#include "./OPengine.h"

#include "GameMessage.h"
#include "messages\CreateMatchResult.h"

#pragma comment(lib, "Ws2_32.lib")

#include "RakNetTypes.h"

#include "Game.h"

#define PLAYER_MAX 2
#define GAME_MATCH_MAX 10

struct GameMatch {
	Player players[PLAYER_MAX];
	ui32 playerIndex;

	void Init() {
		playerIndex = 0;
	}

	ui32 AddPlayer(RakNet::RakNetGUID guid) {
		players[playerIndex].guid = guid;
		return playerIndex++;
	}

	ui32 RemovePlayer(RakNet::RakNetGUID guid) {
		for (ui32 i = 0; i < PLAYER_MAX; i++) {
			if (players[i].guid == guid) {
				players[i] = players[playerIndex - 1];
				playerIndex--;
				return 1;
			}
		}

		return 0;
	}
};

struct GameServer {
	RakNet::RakPeerInterface* server;
	RakNet::Packet* p;
	RakNet::SystemAddress clientID;
	ui8 packetIdentifier;

	GameMatch gameMatch[GAME_MATCH_MAX];
	ui32 gameMatchIndex;

	void(*MessageHandler)(GameMessage, ui8*, ui32, RakNet::RakNetGUID);

	i8 Start(ui16 port);
	void Update(OPtimer* timer);
	void Send(GameMessage message, ui8* data, ui32 length);
	void Send(GameMessage message, ui8* data, ui32 length, RakNet::RakNetGUID guid);
	void Send(GameMessage message, ui8* data, ui32 length, RakNet::RakNetGUID guid, i8 exclude);
	void HandleMessage(ui8* data, ui32 length, RakNet::RakNetGUID guid);
	void HandleDisconnect(RakNet::RakNetGUID guid);
};

extern GameServer SERVER;