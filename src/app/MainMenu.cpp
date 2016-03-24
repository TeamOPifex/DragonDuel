#include "MainMenu.h"
#include "GameState.h"

OPfontManager* FontManager;
ui8 sel = 0;
OPvec4 color = { 1, 1, 1, 1 };
OPvec4 selected = { 0.4, 0.4, 1, 1 };
OPvec4 background = { 0.1, 0.1, 0.1, 1 };

void MainMenuInit(OPgameState* last) {

	OPcmanLoad("cookies.opf");
	OPfontSystemLoadEffects();

	FontManager = OPfontManagerSetup("cookies.opf", NULL, 0);
}

OPint MainMenuUpdate(OPtimer* time) {

	// UP
	if (OPkeyboardWasPressed(OPKEY_W)) {
		sel -= 1;
		sel %= 2;
	}

	// DOWN
	if (OPkeyboardWasPressed(OPKEY_S)) {
		sel += 1;
		sel %= 2;
	}

	// SELECT
	if (OPkeyboardWasPressed(OPKEY_ENTER)) {
		if (sel == 0) {
			GS_MAIN_MENU.Data = "1338";
			OPgameStateChange(&GS_EXAMPLE);
		}
		else {
			GS_MAIN_MENU.Data = "1339";
			OPgameStateChange(&GS_EXAMPLE);
		}
	}

	return false;
}

void MainMenuRender(OPfloat delta) {
	OPrenderClear(background);

	OPfontRenderBegin(FontManager);
	FontManager->scale = 2.0;
	OPfontRender("Dragon Duel", OPvec2Create(50, 50));

	FontManager->scale = 1.0;
	OPfontColor(sel == 0 ? selected : color);
	OPfontRender("Single Player", OPvec2Create(50, 150));
	OPfontColor(sel == 1 ? selected : color);
	OPfontRender("Multiplayer", OPvec2Create(50, 200));
	OPfontRenderEnd();

	OPrenderPresent();
}

OPint MainMenuExit(OPgameState* next) {
	return 0;
}


OPgameState GS_MAIN_MENU = {
	MainMenuInit,
	MainMenuUpdate,
	MainMenuRender,
	MainMenuExit
};
