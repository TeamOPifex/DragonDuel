#include "MultiPlayer.h"
#include "GameState.h"
#include "Client.h"

void keystream(OPchar c);
void MultiHandle(GameMessage message, ui8* data);

struct MultiPlayer {
	OPeffect effect;
	OPvec3 background = { 0.1f, 0.1f, 0.6f };
	ui8 textActive;
	OPchar buffer[100];
	OPfontManager* FontManager;
	OPimgui* imgui;
	OPint textTimer;
	i8 connected;

	void Init() {
		OPKEYBOARD_STREAM = keystream;
		MessageHandler = MultiHandle;
		
		buffer[0] = NULL;
		textActive = 0;
		textTimer = 0;
		connected = 0;

		FontManager = OPfontManagerSetup("Ubuntu.opf", NULL, 0);
		FontManager->scale = 1.0;

		effect = OPeffectGen(
			"imgui.vert",
			"imgui.frag",
			OPATTR_POSITION | OPATTR_COLOR4,
			"IMGUI Shader",
			sizeof(OPfloat) * 7
			);
		imgui = OPimguiCreate(&effect, FontManager);
		imgui->primaryColor = OPvec4Create(1, 0, 0, 1);
		imgui->secondaryColor = OPvec4Create(1, 1, 1, 1);
		imgui->hoverColor = OPvec4Create(0.9f, 0.9f, 0.9f, 1);
		imgui->padding = OPvec4Create(10, 10, 10, 10);

		OPimguiBind(imgui);
	}

	OPint Update(OPtimer* timer) {
		textTimer += timer->Elapsed;
		if (!connected) return 0;

		if (OPkeyboardWasPressed(OPKEY_SPACE)) {
			OPvec3 color = OPvec3Create(0, 1, 0);
			ClientSend(BackgroundColor, (i8*)&color, sizeof(color));
		}
		ClientUpdate();

		return 0;
	}

	void Render(OPfloat delta) {
		OPrenderClear(background);

		if (!connected) {

			OPimguiBegin();

			if (OPimguiButton(OPvec2Create(440, 20), "Join")) {
				connected = 1;
				ClientStart(buffer, "1337", "127.0.0.1");
			}

			OPint state = OPimguiTextbox(OPvec2Create(20, 20), buffer, "IP Address", textActive, textTimer % 500 > 250);
			if (state == 1) textActive = true;
			else if (state == 2) textActive = false;

			OPimguiEnd();

		}
		else {
			// Multiplayer state is now running
		}


		OPrenderPresent();
	}

	OPint Exit() {
		return 0;
	}
};

MultiPlayer multiPlayer;

void keystream(OPchar c) {
	if (!multiPlayer.textActive) return;
	OPint p = strlen(multiPlayer.buffer);
	multiPlayer.buffer[p] = c;
	multiPlayer.buffer[p + 1] = '\0';
	OPlog("MyCode %c", c);
}

void MultiHandle(GameMessage message, ui8* data) {
	if (message == BackgroundColor) {
		multiPlayer.background = *(OPvec3*)&data[1];
	}
}



void MultiPlayerInit(OPgameState* last) {
	multiPlayer.Init();
}

OPint MultiPlayerUpdate(OPtimer* time) {
	return multiPlayer.Update(time);
}

void MultiPlayerRender(OPfloat delta) {
	multiPlayer.Render(delta);
}

OPint MultiPlayerExit(OPgameState* next) {
	return multiPlayer.Exit();
}

OPgameState GS_MULTI_PLAYER = {
	MultiPlayerInit,
	MultiPlayerUpdate,
	MultiPlayerRender,
	MultiPlayerExit
};
