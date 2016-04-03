//////////////////////////////////////
// Application Entry Point
//////////////////////////////////////
#include "./OPengine.h"
#include "Main.h"
#include "MainMenu.h"

//////////////////////////////////////
// Application Methods
//////////////////////////////////////

void ApplicationInit() {
	OP_LOG_LEVEL = 999;

	OPloadersAddDefault();
	OPcmanInit(OPIFEX_ASSETS);
	OPrenderInit();
	OPgameStateChange(&GS_MAIN_MENU);

}

OPint ApplicationUpdate(OPtimer* timer) {
	OPinputSystemUpdate(timer);

#ifdef _DEBUG
	if (OPkeyboardWasPressed(OPKEY_ESCAPE)) {
		return 1;
	}
#endif

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

	OP_MAIN_START_STEPPED
	OP_MAIN_END
	OP_MAIN_SUCCESS
}
