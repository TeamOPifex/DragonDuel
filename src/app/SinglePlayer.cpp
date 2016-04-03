#include "SinglePlayer.h"
#include "GameState.h"
#include "Client.h"
#include "Server.h"
#include "Game.h"
#include "messages\CreateMatchResult.h"

void Handle(GameMessage message, ui8* data, ui32 length);

struct SinglePlayer {
	OPvec3 background = { 0.6f, 0.1f, 0.1f };
	Game game;
	Player* player;
	ui8 Started;

	void Init() {
		Started = 0;
		SERVER.Start(1337);
		CLIENT.Start(1338, 1337, "127.0.0.1");
		CLIENT.MessageHandler = Handle;
		game.Init();
	}

	OPint Update(OPtimer* timer) {
		if (!Started) return 0;

		if (OPkeyboardWasPressed(OPKEY_SPACE)) {
			OPvec3 color = OPvec3Create(0, 0.2, 0);
			CLIENT.Send(BackgroundColor, (ui8*)&color, sizeof(color));
			CLIENT.Send(CreateMatch);
		}
		SERVER.Update(timer); 
		CLIENT.Update(timer);
		player->UpdateInput(timer);
		game.Update(timer);
		return false;
	}

	void Render(OPfloat delta) {
		OPrenderClear(background);
		game.Render(delta);
		OPrenderPresent();
	}

	OPint Exit() {
		return 0;
	}
};

SinglePlayer singlePlayer;

void Handle(GameMessage message, ui8* data, ui32 length) {
	switch (message) {
		case BackgroundColor: {
			singlePlayer.background = *(OPvec3*)data;
			break;
		}
		case CreateMatch: {
			CreateMatchResult* result = (CreateMatchResult*)data;
			JoinMatchMessage msg = {
				result->matchIndex
			};
			CLIENT.Send(JoinMatch, (ui8*)&msg, sizeof(JoinMatchMessage));
			break;
		}
		case JoinMatch: {
			JoinMatchResult* result = (JoinMatchResult*)data;
			singlePlayer.player = singlePlayer.game.AddPlayer(result->guid);
			singlePlayer.Started = 1;
			OPlog("Joined a game with %d other players", result->otherPlayers);
			break;
		}
	}
}



void SinglePlayerInit(OPgameState* last) {
	singlePlayer.Init();
}

OPint SinglePlayerUpdate(OPtimer* time) {
	return singlePlayer.Update(time);
}

void SinglePlayerRender(OPfloat delta) {
	singlePlayer.Render(delta);
}

OPint SinglePlayerExit(OPgameState* next) {
	return singlePlayer.Exit();
}

OPgameState GS_SINGLE_PLAYER = {
	SinglePlayerInit,
	SinglePlayerUpdate,
	SinglePlayerRender,
	SinglePlayerExit
};
