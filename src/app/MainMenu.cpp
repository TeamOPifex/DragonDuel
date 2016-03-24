#include "MainMenu.h"
#include "SinglePlayer.h"
#include "MultiPlayer.h"

struct MainMenu {
	OPfontManager* FontManager;
	ui8 sel = 0;
	OPvec4 color = { 1, 1, 1, 1 };
	OPvec4 selected = { 0.4f, 0.4f, 1, 1 };
	OPvec4 background = { 0.1f, 0.1f, 0.1f, 1 };

	void Init() {
		OPcmanLoad("cookies.opf");
		OPfontSystemLoadEffects();

		FontManager = OPfontManagerSetup("cookies.opf", NULL, 0);
	}

	OPint Update(OPtimer* timer) {
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
				OPgameStateChange(&GS_SINGLE_PLAYER);
			}
			else {
				OPgameStateChange(&GS_MULTI_PLAYER);
			}
		}

		return false;
	}

	void Render(OPfloat delta) {
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

	OPint Exit() {
		return 0;
	}
};




MainMenu mainMenu;
void MainMenuInit(OPgameState* last) {
	mainMenu.Init();
}

OPint MainMenuUpdate(OPtimer* time) {
	return mainMenu.Update(time);
}

void MainMenuRender(OPfloat delta) {
	mainMenu.Render(delta);
}

OPint MainMenuExit(OPgameState* next) {
	return mainMenu.Exit();
}

OPgameState GS_MAIN_MENU = {
	MainMenuInit,
	MainMenuUpdate,
	MainMenuRender,
	MainMenuExit
};
