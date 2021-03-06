
#include "Application.h"

//Include GLEW
//#include <GL/glew.h>

//Include GLFW
//#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

#include "SceneManager.h"

#include "MainScene.h"
#include "BossScene.h"
#include "MainMenu.h"
#include "UndergroundScene.h"
#include "MiniGame.h"
#include "WorldScene.h"
#include "InsideBarrackScene.h"
#include "SplashScreen.h"
#include "LoadingScreen.h"

#include "DetectMemoryLeak.h"

GLFWwindow* Application::m_window = 0;
SceneManager* SceneManager::Instance = 0;
const unsigned char Application::FPS = 60; // FPS of this game
const unsigned int Application::frameTime = 1000 / FPS; // time for each frame

//change window size here
int Application::windowWidth = 1024;
int Application::windowHeight = 768;
StopWatch Application::m_timer;

std::vector<Scene*> SceneManager::scenes;
int SceneManager::currSceneID = 0;
int SceneManager::nextSceneID = 0;

Scene::Text_Data Scene::Text[Scene::TEXT_TYPE::Text_Count];
unsigned Scene::m_parameters[U_TOTAL];
unsigned Scene::m_programID;

//SET DEBUGMODE TO TRUE OR FALSE HERE==============================
bool Scene::debugMode = false;

bool Application::closeWindow = false;

//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Memory leak
struct AtExit
{
	~AtExit() { _CrtDumpMemoryLeaks(); }
} doAtExit;

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

bool Application::IsKeyPressed(unsigned short key)
{
    return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

Application::Application()
{
}

Application::~Application()
{
}

void resize_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);	//Update opengl the new window size
	Application::setWindowSize(w, h);
}

void Application::Init()
{
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 


	//Create a window and create its OpenGL context
	m_window = glfwCreateWindow(windowWidth, windowHeight, "Computer Graphics", NULL, NULL);

	//If the window couldn't be created
	if (!m_window)
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetWindowSizeCallback(m_window, resize_callback);

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	//Sets the key callback
	//glfwSetKeyCallback(m_window, key_callback);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK) 
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}
}

void Application::Run()
{

	//Change the enum in scene Manager too.
	Scene *tempScene;
	tempScene = new SplashScreen();	        //0
	SceneManager::getInstance()->AddScene(tempScene);
	tempScene = new MainMenu();	            //1
	SceneManager::getInstance()->AddScene(tempScene);
	tempScene = new MainScene();			//2
	SceneManager::getInstance()->AddScene(tempScene);
	tempScene = new WorldScene();			//3
	SceneManager::getInstance()->AddScene(tempScene);
	tempScene = new InsideBarrackScene();	//4
	SceneManager::getInstance()->AddScene(tempScene);
	tempScene = new BossScene();			//5
	SceneManager::getInstance()->AddScene(tempScene);
	tempScene = new UndergroundScene();		//6
	SceneManager::getInstance()->AddScene(tempScene);
	tempScene = new MiniGame();				//7
	SceneManager::getInstance()->AddScene(tempScene);
	tempScene = new LoadingScreen();		//8
	SceneManager::getInstance()->AddScene(tempScene);


//	SceneManager::getInstance()->SetNextSceneID(1);
	//SceneManager::getInstance()->SetNextScene();

	//Scene *scene = scene1;
	SceneManager::getInstance()->getCurrentScene()->Init();

	//Scene *scene = new A3Scene();

	//set cursor pos
	glfwSetCursorPos(m_window, windowWidth / 2, windowHeight / 2);
	// Hide the mouse and enable unlimited mouvement
	//glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	while (!glfwWindowShouldClose(m_window) && !closeWindow)
	{
		SceneManager::getInstance()->Update();
	} //Check if the ESC key had been pressed or if the window had been closed
	SceneManager::getInstance()->~SceneManager();




	//delete scene;
}

void Application::Exit()
{
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}
