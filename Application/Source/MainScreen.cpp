#include "GL\glew.h"
#include <GLFW\glfw3.h>

#include "MainScreen.h"
#include "MeshBuilder.h"
#include "MainScene.h"
#include "RenderMesh.h"
#include "Vector3.h"

//Mesh* MainScreen::startButton = 0;

MainScreen::MainScreen()
{
}

MainScreen::~MainScreen()
{
}

void MainScreen::Init()
{
	meshList[MAIN_SCREEN] = MeshBuilder::GenerateQuad("Screen", Color(1, 1, 0), 1, 1);
	startButton = MeshBuilder::GenerateQuad("Screen", Color(1, 0, 0), 1, 1);
	meshList[START_BUTTON] = startButton;

	wasLeftMouseButtonPressed = false;
	isMainMenu = true;
}


void MainScreen::Update(double dt)
{
	double x, y;
	glfwGetCursorPos(Application::m_window, &x, &y);

	leftButton = glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT);
	isLeftMouseButtonPressed = leftButton;


	startButton->setHb(true, Vector3(200, 367, 0), Vector3(400, 567, 2), Vector3(0, 0, 0), Vector3(0, 1, 0));

	if (isLeftMouseButtonPressed && !wasLeftMouseButtonPressed)
	{
		if (startButton->isPointInsideAABB(Position(x, y, 0)))
			isMainMenu = false;

		wasLeftMouseButtonPressed = isLeftMouseButtonPressed;
	}

	if (!isLeftMouseButtonPressed && wasLeftMouseButtonPressed)
		wasLeftMouseButtonPressed = isLeftMouseButtonPressed;
}

void MainScreen::Render()
{
	// Render VBO here

	RenderMeshClass::RenderMeshOnScreen(meshList[MAIN_SCREEN], 1, 1, 1, 1024, 768, &projectionStack, &viewStack, &modelStack, Scene::m_parameters);
	RenderMeshClass::RenderMeshOnScreen(meshList[START_BUTTON], 300, 300, 2, 200, 200, &projectionStack, &viewStack, &modelStack, Scene::m_parameters);
	//	renderEnvironment();

}
