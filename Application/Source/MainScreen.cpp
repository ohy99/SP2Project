#include "GL\glew.h"
#include <GLFW\glfw3.h>

#include "MainScreen.h"
#include "MeshBuilder.h"
#include "MainScene.h"
#include "RenderMesh.h"
#include "Application.h"
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
	meshList[MAIN_SCREEN]->textureID = LoadTGA("Image//main_menu.tga");
	startButton = MeshBuilder::GenerateQuad("Screen", Color(1, 0, 0), 1, 1);
	meshList[START_BUTTON] = startButton;
	//startButton->textureID = LoadTGA("Image//Assignment//model_texture//Tree.tga");

	wasLeftMouseButtonPressed = false;
	isMainMenu = true;
}


void MainScreen::Update(double dt)
{
	double x, y;
	glfwGetCursorPos(Application::m_window, &x, &y);

	leftButton = glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT);
	isLeftMouseButtonPressed = leftButton;

	Vector3 min = Vector3(((float)Application::getWindowWidth() / 1024.f) *  -100.f, (((float)Application::getWindowHeight() / 768.f) *  -100.f), 0.f);
	Vector3 max = Vector3((((float)Application::getWindowWidth() / 1024.f) *  100.f), (((float)Application::getWindowHeight() / 768.f) * 100.f), 2.f);
	Vector3 pos = Vector3((float)(Application::getWindowWidth() / 1024.f) * 300.f, (float)(Application::getWindowHeight() / 768.f) * 467.f, 0.f);

	startButton->setHb(true, min , max, pos, Vector3(0.f, 1.f, 0.f));

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

	RenderMeshClass::RenderMeshOnScreen(meshList[MAIN_SCREEN], (float)Application::getWindowWidth() * 0.5f, (float)Application::getWindowHeight() * 0.5f, 1.f, (float)Application::getWindowWidth(), (float)Application::getWindowHeight(), &projectionStack, &viewStack, &modelStack, Scene::m_parameters);
	RenderMeshClass::RenderMeshOnScreen(meshList[START_BUTTON], (float)(Application::getWindowWidth() / 1024.f) * 300.f, (float)(Application::getWindowHeight() / 786.f) * 300.f, 2.f, (float)(Application::getWindowWidth() / 1024.f) * 200.f, (float)(Application::getWindowHeight() / 768.f) * 200.f, &projectionStack, &viewStack, &modelStack, Scene::m_parameters);
	//	renderEnvironment();

}
