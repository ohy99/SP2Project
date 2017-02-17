#include "UI.h"
#include "MeshBuilder.h"
#include "RenderMesh.h"

void UI::Init()
{
	meshList[QUAD] = MeshBuilder::GenerateQuad("image", Color(0.9f, 0.9f, 0.9f), 1.0f, 1.0f);
}

void UI::renderPause(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	RenderMeshClass::RenderMeshOnScreen(meshList[QUAD], 500, 380, 0, 700, 700, projectionStack, viewStack, modelStack, m_parameters);
}