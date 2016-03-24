#include "./GameState.h"
#include "Client.h"
#include "Server.h"

OPvec3 bg = OPVEC3_ZERO;

void Handle(GameMessage message, ui8* data) {
	if (message == BackgroundColor) {
		bg = *(OPvec3*)&data[1];
	}
}

void ExampleStateInit(OPgameState* last) {
	MessageHandler = Handle;
	//ServerStart("1337");
	ClientStart((i8*)last->Data, "1337", "127.0.0.1");
}

OPint ExampleStateUpdate(OPtimer* time) {
	if (OPkeyboardWasPressed(OPKEY_SPACE)) {
		OPvec3 color = OPvec3Create(0, 1, 0);
		ClientSend(BackgroundColor, (i8*)&color, sizeof(color));
	}
	//ServerUpdate();
	ClientUpdate();
	return false;
}

void ExampleStateRender(OPfloat delta) {
	OPrenderClear(bg);
	OPrenderPresent();
}

OPint ExampleStateExit(OPgameState* next) {
	return 0;
}


OPgameState GS_EXAMPLE = {
	ExampleStateInit,
	ExampleStateUpdate,
	ExampleStateRender,
	ExampleStateExit
};
