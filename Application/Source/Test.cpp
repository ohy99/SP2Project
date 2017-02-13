#include "Test.h"
#include "MeshBuilder.h"
#include "MainScene.h"


Test::Test()
{
}

Test::~Test()
{
}

void Test::Init()
{
	meshList[GEO_AXES] = MeshBuilder::GenerateCube("", Color(1, 1, 0), 1, 1, 1);

}


void Test::Update(double dt)
{
}

void Test::Render()
{
	// Render VBO here

	MainScene::RenderMesh(meshList[GEO_AXES], false);

	//	renderEnvironment();

}
