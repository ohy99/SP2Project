#include "MainMenu.h"
#include "GL\glew.h"

#include <GLFW\glfw3.h>

#include "shader.hpp"
#include "Mtx44.h"
#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "LoadTextData.h"
//#include "GameObject.h"

#include "RenderMesh.h"

#include "UI.h"
#include "Player.h"
#include "BossAI.h"
#include "SandStorm.h"

MainMenu::MainMenu()
{
}

MainMenu::~MainMenu()
{
}

void MainMenu::Init()
{
	// Init VBO here

	glClearColor(0.0f, 0.5f, 0.66f, 0.0f);

	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);


	for (size_t i = 0; i < GEOMETRY_TYPE::NUM_GEOMETRY; i++)
		meshList[i] = NULL;


	Player::getInstance();
	GoatBoss::getInstance();
	SandStorm::getInstance();

	camera = new Camera3;
	camera->Init(Vector3(0.f, 0.f, 7.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f));

	meshList[MAIN_SCREEN] = MeshBuilder::GenerateQuad("Screen", Color(1.f, 1.f, 0.f), 1.f, 1.f);
	meshList[MAIN_SCREEN]->textureID = LoadTGA("Image//main_menu.tga");

	meshList[START_BUTTON] = MeshBuilder::GenerateQuad("Start Button", Color(1.f, 0.f, 0.f), 1.f, 1.f);
	meshList[START_BUTTON]->textureID = LoadTGA("Image//mainstart.tga");

	meshList[START_BUTTON2] = MeshBuilder::GenerateQuad("Start Button", Color(1.f, 0.f, 0.f), 1.f, 1.f);
	meshList[START_BUTTON2]->textureID = LoadTGA("Image//mainstart2.tga");

	meshList[QUIT] = MeshBuilder::GenerateQuad("Quit Button", Color(1.f, 0.f, 0.f), 1.f, 1.f);
	meshList[QUIT]->textureID = LoadTGA("Image//mainquit.tga");

	meshList[QUIT2] = MeshBuilder::GenerateQuad("Quit Button", Color(1.f, 0.f, 0.f), 1.f, 1.f);
	meshList[QUIT2]->textureID = LoadTGA("Image//mainquit2.tga");


	wasLeftMouseButtonPressed = false;
	isStartPressed = false;

	min = Vector3((float)(Application::getWindowWidth() / 1024.f) * -200.f, (float)(Application::getWindowHeight() / 768.f) * -50.f, 0.f);
	max = Vector3((float)(Application::getWindowWidth() / 1024.f) * 200.f, (float)(Application::getWindowHeight() / 768.f) * 50.f, 2.f);
	pos = Vector3((float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 768.f) * (Application::getWindowHeight() - 395.f), 0.f);

	meshList[START_BUTTON]->setHb(true, min, max, pos, Vector3(0.f, 1.f, 0.f));
	meshList[QUIT]->setHb(true, Vector3((Application::getWindowWidth() / 1024.f) * -200.f, (float)(Application::getWindowHeight() / 768.f) * -50.f, 0.f),
		Vector3((float)(Application::getWindowWidth() / 1024.f) * 200.f, (float)(Application::getWindowHeight() / 768.f) * 50.f, 2.f),
		Vector3((float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 768.f) * (Application::getWindowHeight() - 250.f), 0.f),
		Vector3(0.f, 1.f, 0.f));

	quit = false;
	start = false;

	//Mtx44 projection;
	//projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	//projectionStack.LoadMatrix(projection);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void MainMenu::Update(double dt)
{
	glfwGetCursorPos(Application::m_window, &x, &y);

	leftButton = glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT);
	isLeftMouseButtonPressed = leftButton;

	if (isLeftMouseButtonPressed && !wasLeftMouseButtonPressed)
	{

		if (meshList[START_BUTTON]->isPointInsideAABB(Position((float)x, (float)y, 0.f)))
			isStartPressed = true;

		if (meshList[QUIT]->isPointInsideAABB(Position((float)x, (float)y, 0.f)))
			Application::setCloseWindow(true);

		wasLeftMouseButtonPressed = isLeftMouseButtonPressed;
	}

	if (!isLeftMouseButtonPressed && wasLeftMouseButtonPressed)
		wasLeftMouseButtonPressed = isLeftMouseButtonPressed;
	
	if (meshList[START_BUTTON]->isPointInsideAABB(Position((float)x, (float)y, 0.f)))
		start = true;
	else
		start = false;
	
	if (meshList[QUIT]->isPointInsideAABB(Position((float)x, (float)y, 0.f)))
		quit = true;
	else
		quit = false;

	if (isStartPressed)
	{
		SceneManager::getInstance()->SetNextSceneID(SceneManager::SCENES::CAMPSCENE);
		SceneManager::getInstance()->SetNextScene();
	}

	FramesPerSec = 1 / dt;
}

void MainMenu::Render()
{
	// Render VBO here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera->getPosition().x, camera->getPosition().y, camera->getPosition().z,
		camera->getPosition().x + camera->getDir().x, camera->getPosition().y + camera->getDir().y, camera->getPosition().z + camera->getDir().z,
		camera->getUp().x, camera->getUp().y, camera->getUp().z);
	modelStack.LoadIdentity();

	RenderMeshClass::RenderMeshOnScreen(meshList[MAIN_SCREEN], (float)Application::getWindowWidth() * 0.5f, (float)Application::getWindowHeight() * 0.5f, 1.f, (float)Application::getWindowWidth(), (float)Application::getWindowHeight(), &projectionStack, &viewStack, &modelStack, Scene::m_parameters);

	if (!start)
		RenderMeshClass::RenderMeshOnScreen(meshList[START_BUTTON], (float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 786.f) * 400.f, 2.f, (float)(Application::getWindowWidth() / 1024.f) * 400.f, (float)(Application::getWindowHeight() / 768.f) * 100.f, &projectionStack, &viewStack, &modelStack, Scene::m_parameters);

	else
		RenderMeshClass::RenderMeshOnScreen(meshList[START_BUTTON2], (float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 786.f) * 400.f, 2.f, (float)(Application::getWindowWidth() / 1024.f) * 400.f, (float)(Application::getWindowHeight() / 768.f) * 100.f, &projectionStack, &viewStack, &modelStack, Scene::m_parameters);

	if (!quit)
		RenderMeshClass::RenderMeshOnScreen(meshList[QUIT], (float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 768.f) * 250.f, 2.f, (float)(Application::getWindowWidth() / 1024.f) * 400.f, (float)(Application::getWindowHeight() / 768.f) * 100.f, &projectionStack, &viewStack, &modelStack, Scene::m_parameters);

	else
		RenderMeshClass::RenderMeshOnScreen(meshList[QUIT2], (float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 768.f) * 250.f, 2.f, (float)(Application::getWindowWidth() / 1024.f) * 400.f, (float)(Application::getWindowHeight() / 768.f) * 100.f, &projectionStack, &viewStack, &modelStack, Scene::m_parameters);
}

void MainMenu::Exit()
{
	for (size_t i = 0; i < GEOMETRY_TYPE::NUM_GEOMETRY; i++)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	delete camera;

	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}