#include "Game.h"

void Game::Init() {
	playerCount = 0;

	//OPmeshBuilder* builder = OPmeshBuilderCreate(sizeof(OPfloat) * 6);

	//MeshBuilderVert one = { -1, 1, 0, 1, 0, 0 };
	//MeshBuilderVert two = { 1, 1, 0, 1, 0, 0 };
	//MeshBuilderVert three = { 1, -1, 0, 1, 0, 0 };
	//MeshBuilderVert four = { -1, -1, 0, 1, 0, 0 };
	//OPmeshBuilderAdd(builder, &one, &two, &three, &four);

	//Mesh = OPmeshBuilderGen(builder);
	//OPmeshBuilderDestroy(builder);

 	Mesh = *((OPmesh*)OPcmanLoadGet("spyro.opm"));

	Effect = OPeffectGen(
		"ColoredModel.vert",
		"ColoredModel.frag",
		OPATTR_POSITION | OPATTR_COLOR,
		"MeshBuilder Effect",
		Mesh.vertexLayout.stride);

	Camera = OPcamPersp(
		OPvec3Create(0, 50, 50),
		OPVEC3_UP,
		OPVEC3_UP,
		0.1f,
		1000.0f,
		45.0f,
		OPRENDER_WIDTH / (f32)OPRENDER_HEIGHT
		);

	OPrenderDepth(1);
	OPrenderCull(0);
}