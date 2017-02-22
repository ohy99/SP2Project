#include "UI.h"
#include "MeshBuilder.h"
#include "RenderMesh.h"
#include "Application.h"

void UI::Init()
{
	meshList[QUAD] = MeshBuilder::GenerateQuad("image", Color(0.9f, 0.9f, 0.9f), 1.0f, 1.0f);
}

void UI::renderPause(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	RenderMeshClass::RenderMeshOnScreen(meshList[QUAD], (int)Application::getWindowWidth() * 0.5, (int)Application::getWindowHeight() * 0.5, 0, (int)(Application::getWindowWidth() / 1024) * 700, (int)(Application::getWindowHeight() / 768) * 700, projectionStack, viewStack, modelStack, m_parameters);
}