#pragma once

#include "./OPengine.h"
#include "Client.h"

struct Player {
	OPvec3 Position;
	OPvec3 PrevPosition;
	RakNet::RakNetGUID guid;

	void Update(OPtimer* timer) {
	}

	void UpdateInput(OPtimer* timer) {
		PrevPosition = Position;
		if (OPkeyboardIsDown(OPKEY_A)) {
			Position.x -= 0.1 * timer->Elapsed;
			Position.y = 0;

			CLIENT.Send(PositionUpdate, (ui8*)this, sizeof(Player));
		}
		if (OPkeyboardIsDown(OPKEY_D)) {
			Position.x += 0.1 * timer->Elapsed;
			Position.y = 0;
			CLIENT.Send(PositionUpdate, (ui8*)this, sizeof(Player));
		}
		if (OPkeyboardIsDown(OPKEY_W)) {
			Position.z -= 0.1 * timer->Elapsed;
			Position.y = 0;
			CLIENT.Send(PositionUpdate, (ui8*)this, sizeof(Player));
		}
		if (OPkeyboardIsDown(OPKEY_S)) {
			Position.z += 0.1 * timer->Elapsed;
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
		OPrenderCull(0);
		OPrenderDepth(1);
		for (ui8 i = 0; i < playerCount; i++) {
			OPmat4 world = OPmat4RotY(OPpi_2 + (2 * i * OPpi_2));
			world.Translate(OPvec3Tween(players[i].PrevPosition, players[i].Position, delta));
			OPbindMeshEffectWorldCam(&Mesh, &Effect, &world, &Camera);
			OPmeshRender();
		}
	}
};