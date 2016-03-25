#include "SinglePlayer.h"
#include "GameState.h"
#include "Client.h"
#include "Server.h"


void Handle(GameMessage message, ui8* data);

struct SinglePlayer {
	OPvec3 background = { 0.6f, 0.1f, 0.1f };

	void Init() {
		MessageHandler = Handle;
		ServerStart(1337);
		ClientStart(1338, 1337, "127.0.0.1");
	}

	OPint Update(OPtimer* timer) {
		if (OPkeyboardWasPressed(OPKEY_SPACE)) {
			OPvec3 color = OPvec3Create(0, 1, 0);
			ClientSend(BackgroundColor, (i8*)&color, sizeof(color));
		}
		ServerUpdate();
		ClientUpdate();
		return false;
	}

	void Render(OPfloat delta) {
		OPrenderClear(background);

		OPrenderPresent();
	}

	OPint Exit() {
		return 0;
	}
};

SinglePlayer singlePlayer;

void Handle(GameMessage message, ui8* data) {
	if (message == BackgroundColor) {
		singlePlayer.background = *(OPvec3*)&data[1];
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
