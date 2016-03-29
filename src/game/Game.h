#pragma once

#include "./OPengine.h"
#include "Client.h"

struct Player {
	OPvec3 Position;
	RakNet::RakNetGUID guid;

	void Update(OPtimer* timer) {
	}

	void UpdateInput(OPtimer* timer) {
		if (OPkeyboardWasPressed(OPKEY_A)) {
			Position.x -= 1;
			Position.y = 0;

			CLIENT.Send(PositionUpdate, (ui8*)this, sizeof(Player));
		}
		if (OPkeyboardWasPressed(OPKEY_D)) {
			Position.x += 1;
			Position.y = 0;
			CLIENT.Send(PositionUpdate, (ui8*)this, sizeof(Player));
		}
		if (OPkeyboardWasPressed(OPKEY_W)) {
			Position.z -= 1;
			Position.y = 0;
			CLIENT.Send(PositionUpdate, (ui8*)this, sizeof(Player));
		}
		if (OPkeyboardWasPressed(OPKEY_S)) {
			Position.z += 1;
			Position.y = 0;
			CLIENT.Send(PositionUpdate, (ui8*)this, sizeof(Player));
		}
	}
};

struct MeshBuilderVert {
	OPvec3 pos;
	OPvec3 col;
};

struct Game {

	Player players[2];
	ui8 playerCount;
	OPmesh Mesh;
	OPeffect Effect;
	OPcam Camera;

	void Init();

	Player* AddPlayer(RakNet::RakNetGUID guid) {
		Player* result = &players[playerCount++];
		result->Position = OPVEC3_ZERO;
		result->guid = guid;
		return result;
	}

	void RemovePlayer(RakNet::RakNetGUID guid) {
		for (ui32 i = 0; i < playerCount; i++) {
			if (players[i].guid == guid) {
				players[0] = players[playerCount - 1];
				playerCount--;
			}
		}
	}

	OPint Update(OPtimer* timer) {
		for (ui32 i = 0; i < playerCount; i++) {
			players[i].Update(timer);
		}
		return 0;
	}

	void Render(OPfloat delta) {
		for (ui8 i = 0; i < playerCount; i++) {
			OPmat4 world = OPmat4Translate(players[i].Position);
			OPbindMeshEffectWorldCam(&Mesh, &Effect, &world, &Camera);
			OPmeshRender();
		}
	}
};