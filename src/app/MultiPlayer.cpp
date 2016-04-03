#include "MultiPlayer.h"
#include "GameState.h"
#include "Client.h"
#include "messages\CreateMatchResult.h"

void keystream(OPchar c);
void MultiHandle(GameMessage message, ui8* data, ui32 length);

const OPchar* loopback = "127.0.0.1";
const OPchar* port = "1337";

struct MultiPlayer {
	OPeffect effect;
	OPvec3 background = { 0.1f, 0.1f, 0.6f };
	ui8 textActive;
	ui8 textPortActive;
	OPchar buffer[100];
	OPchar buffer2[100];
	OPfontManager* FontManager;
	OPimgui* imgui;
	OPint textTimer;
	i8 connected;
	Game game;
	Player* player;
	i8 CREATED_MATCH;
	i8 GAME_STARTED;

	void Init() {
		CREATED_MATCH = 0;
		GAME_STARTED = 0;
		game.Init();
		player = NULL;

		OPKEYBOARD_STREAM = keystream;

		OPmemcpy(buffer, loopback, strlen(loopback) + 1);

		OPmemcpy(buffer2, port, strlen(port) + 1);

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
		
		if (!connected) {
			if (textActive && OPkeyboardWasPressed(OPKEY_BACKSPACE)) {
				OPint p = strlen(buffer);
				if (p > 0) {
					buffer[p - 1] = '\0';
				}
			} else if (textPortActive && OPkeyboardWasPressed(OPKEY_BACKSPACE)) {
				OPint p = strlen(buffer2);
				if (p > 0) {
					buffer2[p - 1] = '\0';
				}
			}
			return 0;
		}

		CLIENT.Update(timer);

		player->UpdateInput(timer);
		game.Update(timer);

		return 0;
	}

	void Render(OPfloat delta) {
		OPrenderClear(background);

		if (!connected) {

			OPimguiBegin();

			if (OPimguiButton(OPvec2Create(440, 20), "Join")) {
				connected = 1;
				ui8 attempt = 0;
				while (attempt < 5 && !CLIENT.Start(1338 + attempt, 1337, buffer)) {
					attempt++;
				}

				CLIENT.MessageHandler = MultiHandle;
			}

			OPint state = OPimguiTextbox(OPvec2Create(20, 20), buffer, "IP Address", textActive, textTimer % 500 > 250);
			if (state == 1) textActive = true;
			else if (state == 2) textActive = false;

			state = OPimguiTextbox(OPvec2Create(20, 120), buffer2, "Port", textPortActive, textTimer % 500 > 250);
			if (state == 1) textPortActive = true;
			else if (state == 2) textPortActive = false;

			OPimguiEnd();

		}
		else if (!GAME_STARTED) {

			if (CLIENT.Connected == 0) {
				OPfontRenderBegin(FontManager);
				OPfontRender("Connecting...", OPvec2Create(0, 250));
				OPfontRenderEnd();
			}
			else if(CLIENT.Connected == -1) {
				OPfontRenderBegin(FontManager);
				background = OPvec3Create(0.8, 0, 0);
				OPfontRender("Failed to connect.", OPvec2Create(0, 250));
				OPfontRenderEnd();

				OPimguiBegin();
				if (OPimguiButton(OPvec2Create(440, 120), "Retry")) {
					connected = 1;
					ui8 attempt = 0;
					while (attempt < 5 && !CLIENT.Start(1338 + attempt, atoi(buffer2), buffer)) {
						attempt++;
					}
					CLIENT.MessageHandler = MultiHandle;
				}
				OPimguiEnd();
			}
			else {

				OPimguiBegin();

				if (OPimguiButton(OPvec2Create(440, 20), "Create Match")) {
					OPvec3 color = OPvec3Create(0, 1, 0);
					CLIENT.Send(BackgroundColor, (ui8*)&color, sizeof(color));
					CLIENT.Send(CreateMatch);
					GAME_STARTED = 1;
				}

				if (OPimguiButton(OPvec2Create(440, 120), "Join Match")) {
					JoinMatchMessage msg = { 0 };
					CLIENT.Send(JoinMatch, (ui8*)&msg, sizeof(JoinMatchMessage));
					GAME_STARTED = 1;
				}

				OPimguiEnd();
			}

		}
		else {
			// Multiplayer state is now running
			game.Render(delta);
		}


		OPrenderPresent();
	}

	OPint Exit() {
		return 0;
	}
};

MultiPlayer multiPlayer;

void keystream(OPchar c) {
	if (multiPlayer.textActive) {
		OPint p = strlen(multiPlayer.buffer);
		multiPlayer.buffer[p] = c;
		multiPlayer.buffer[p + 1] = '\0';
		OPlog("MyCode %c", c);
	}
	else if (multiPlayer.textPortActive) {
		OPint p = strlen(multiPlayer.buffer2);
		multiPlayer.buffer2[p] = c;
		multiPlayer.buffer2[p + 1] = '\0';
		OPlog("MyCode %c", c);
	}
}

void MultiHandle(GameMessage message, ui8* data, ui32 length
	) {
	switch (message) {
		case BackgroundColor: {
			OPlog("BackgroundColor");
			multiPlayer.background = *(OPvec3*)data;
			break;
		}
		case CreateMatch: {
			OPlog("CreateMatch");
			multiPlayer.CREATED_MATCH = 1;
			CreateMatchResult* result = (CreateMatchResult*)data;
			JoinMatchMessage msg = {
				result->matchIndex
			};
			CLIENT.Send(JoinMatch, (ui8*)&msg, sizeof(JoinMatchMessage));
			break;
		}
		case JoinMatch: {
			OPlog("JoinMatch");
			JoinMatchResult* result = (JoinMatchResult*)data;

			for (ui32 i = 0; i < result->otherPlayers; i++) {
				Player* player = multiPlayer.game.AddPlayer(result->players[i].guid);
				player->Position = result->players[i].Position;
			}

			multiPlayer.player = multiPlayer.game.AddPlayer(result->guid);

			OPlog("Joined a game with %d other players", result->otherPlayers);
			
			break;
		}
		case SomeoneJoined: {
			OPlog("SomeoneJoined");

			SomeoneJoinedResult* result = (SomeoneJoinedResult*)data;

			multiPlayer.game.AddPlayer(result->guid);
			break;
		}
		case SomeoneLeft: {
			OPlog("SomeoneLeft");

			SomeoneJoinedResult* result = (SomeoneJoinedResult*)data;

			multiPlayer.game.RemovePlayer(result->guid);
			break;
		}
		case PositionUpdate: {
			OPlog("PositionUpdate");
			Player* player = (Player*)data;
			for (ui32 i = 0; i < multiPlayer.game.playerCount; i++) {
				if (multiPlayer.game.players[i].guid == player->guid) {
					multiPlayer.game.players[i].Position = player->Position;
					break;
				}
			}
			break;
		}
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
