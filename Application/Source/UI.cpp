#include "UI.h"
#include "MeshBuilder.h"

void UI::Init()
{
	meshList[QUAD] = MeshBuilder::GenerateQuad("image", Color(0.9f, 0.9f, 0.9f), 1.0f, 1.0f);
}

void UI::renderPause()
{
	userInterface.RenderMeshOnScreen(meshList[QUAD], 1, 1, 700, 700);
}