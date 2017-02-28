#include "MainScreen.h"
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

MainScreen::MainScreen()
{
}

MainScreen::~MainScreen()
{
}

void MainScreen::Init()
{
	// Init VBO here

	glClearColor(0.0f, 0.5f, 0.66f, 0.0f);

	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Load vertex and fragment shaders
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");

	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");

	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	glUseProgram(m_programID);

	//Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);


	for (size_t i = 0; i < GEOMETRY_TYPE::NUM_GEOMETRY; i++)
		meshList[i] = NULL;

	camera = new Camera3;
	camera->Init(Vector3(0, 0, 7), Vector3(0, 0, 0), Vector3(0, 1, 0));

	meshList[MAIN_SCREEN] = MeshBuilder::GenerateQuad("Screen", Color(1, 1, 0), 1, 1);
	meshList[MAIN_SCREEN]->textureID = LoadTGA("Image//main_menu.tga");
	meshList[START_BUTTON] = MeshBuilder::GenerateQuad("Start Button", Color(1, 0, 0), 1, 1);
	meshList[QUIT] = MeshBuilder::GenerateQuad("Quit Button", Color(1, 0, 0), 1, 1);

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

	//Mtx44 projection;
	//projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	//projectionStack.LoadMatrix(projection);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void MainScreen::Update(double dt)
{
	glfwGetCursorPos(Application::m_window, &x, &y);

	leftButton = glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT);
	isLeftMouseButtonPressed = leftButton;

	if (isLeftMouseButtonPressed && !wasLeftMouseButtonPressed)
	{
		if (meshList[START_BUTTON]->isPointInsideAABB(Position(x, y, 0.f)))
			isStartPressed = true;

		if (meshList[QUIT]->isPointInsideAABB(Position(x, y, 0.f)))
			Application::setCloseWindow(true);

		wasLeftMouseButtonPressed = isLeftMouseButtonPressed;
	}

	if (!isLeftMouseButtonPressed && wasLeftMouseButtonPressed)
		wasLeftMouseButtonPressed = isLeftMouseButtonPressed;

	if (meshList[START_BUTTON]->isPointInsideAABB(Position(x, y, 0.f)))
		meshList[START_BUTTON]->textureID = LoadTGA("Image//mainstart2.tga");

	else
		meshList[START_BUTTON]->textureID = LoadTGA("Image//mainstart.tga");

	if (meshList[QUIT]->isPointInsideAABB(Position(x, y, 0.f)))
		meshList[QUIT]->textureID = LoadTGA("Image//mainquit2.tga");

	else
		meshList[QUIT]->textureID = LoadTGA("Image//mainquit.tga");

	if (isStartPressed)
	{
		SceneManager::getInstance()->SetNextSceneID(1);
		SceneManager::getInstance()->SetNextScene();
	}

	FramesPerSec = 1 / dt;
}

void MainScreen::Render()
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
	RenderMeshClass::RenderMeshOnScreen(meshList[START_BUTTON], (float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 786.f) * 400.f, 2.f, (float)(Application::getWindowWidth() / 1024.f) * 400.f, (float)(Application::getWindowHeight() / 768.f) * 100.f, &projectionStack, &viewStack, &modelStack, Scene::m_parameters);
	RenderMeshClass::RenderMeshOnScreen(meshList[QUIT], (float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 768.f) * 250.f, 2.f, (float)(Application::getWindowWidth() / 1024.f) * 400.f, (float)(Application::getWindowHeight() / 768.f) * 100.f, &projectionStack, &viewStack, &modelStack, Scene::m_parameters);
}

void MainScreen::Exit()
{
	for (size_t i = 0; i < GEOMETRY_TYPE::NUM_GEOMETRY; i++)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	//delete camera;

	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}