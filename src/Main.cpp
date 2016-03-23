//////////////////////////////////////
// Application Entry Point
//////////////////////////////////////
#include "./OPengine.h"
#include "Main.h"
#include "GameState.h"

//////////////////////////////////////
// Application Methods
//////////////////////////////////////

void ApplicationInit() {
	OPloadersAddDefault();
	OPcmanInit(OPIFEX_ASSETS);
	OPrenderInit();
	OPgameStateChange(&GS_EXAMPLE);
}

int ApplicationUpdate(OPtimer* timer) {
	return ActiveState->Update(timer);
}

void ApplicationRender(OPfloat delta) {
	ActiveState->Render(delta);
}

void ApplicationDestroy() {
}

void ApplicationSetup() {
	OPinitialize = ApplicationInit;
	OPupdate = ApplicationUpdate;
	OPrender = ApplicationRender;
	OPdestroy = ApplicationDestroy;
}

//////////////////////////////////////
// Application Entry Point
//////////////////////////////////////
OP_MAIN {
	OPlog("Starting up OPifex Engine");

	ApplicationSetup();

	OP_MAIN_START
	OP_MAIN_END
	OP_MAIN_SUCCESS
}
