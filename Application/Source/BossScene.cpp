#include "BossScene.h"
#include "GL\glew.h"

#include <GLFW\glfw3.h>

#include "shader.hpp"
#include "Mtx44.h"
#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "LoadTextData.h"
#include "LoadATOM.h"
//#include "GameObject.h"

#include "NPC_Doc.h"
#include "Environment.h"

#include "RenderMesh.h"

#include "UI.h"
#include "BossAI.h"

//BossScene::Text_Data BossScene::Text[TEXT_TYPE::Text_Count];
//unsigned BossScene::m_parameters[U_TOTAL];
MS BossScene::modelStack, BossScene::viewStack, BossScene::projectionStack;

//std::vector<GameObject*> BossScene::Game_Objects_(10, NULL);

//std::vector<GameObject*> BossScene::Game_Objects_(10, NULL);
//UI renderMeshOnScreen;
std::vector<EnvironmentObj*> BossScene::Env_Obj;
float BossScene::skyBoxScale = 0.f;
float BossScene::skyBoxDistance = 0.f;
BossScene::BossScene()
{
}

BossScene::~BossScene()
{
}

void BossScene::Init()
{
	// Init VBO here

	glClearColor(0.0f, 0.5f, 0.66f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	

	light[0].position.Set(0.f, 1.f, 2.f); //the rotation is right. but the prob is why there is no light at the x axis
	light[0].color.Set(1.f, 1.f, 1.f);
	light[0].power = 1.f;
	light[0].kC = 0.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].type = Light::LIGHT_POINT;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45.f));
	light[0].cosInner = cos(Math::DegreeToRadian(30.f));
	light[0].exponent = 100.0f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);//in case

	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

	// Make sure you pass uniform parameters after glUseProgram()


	//Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);


	for (size_t i = 0; i < GEOMETRY_TYPE::NUM_GEOMETRY; i++)
		meshList[i] = NULL;


	//Skybox ------------skyBoxScale, skyBoxDistance;
	skyBoxScale = 500;
	skyBoxDistance = skyBoxScale * 0.5f * 0.99f;
	//Left Skybox 
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("Left", Color(1, 1, 1), skyBoxScale, skyBoxScale);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//BossSceneSkyBoxLeft.tga");

	//Right Skybox 
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("Right", Color(1, 1, 1), skyBoxScale, skyBoxScale);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//BossSceneSkyBoxRight.tga");

	//Front Skybox 
	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("Front", Color(1, 1, 1), skyBoxScale, skyBoxScale);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//BossSceneSkyBoxFront.tga");

	//Back Skybox 
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("Back", Color(1, 1, 1), skyBoxScale, skyBoxScale);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//BossSceneSkyBoxBack.tga");

	//Top Skybox 
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("Top", Color(1, 1, 1), skyBoxScale, skyBoxScale);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//BossSceneSkyBoxTop.tga");

	//Bottom Skybox 
	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("Bottom", Color(1, 1, 1), skyBoxScale, skyBoxScale);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//BossSceneSkyBoxBottom.tga");
	//Skybox -------------
	meshList[GROUND] = MeshBuilder::GenerateQuad("Ground Mesh", Color(0.1f, 0.1f, 0.1f), skyBoxScale, skyBoxScale);
//	meshList[GROUND]->textureID = LoadTGA("Image//");


	for (auto it : Env_Obj)
		Player::addCollisionObject(it);

	camera = new Camera3();
	camera->Init(Vector3(0, 0, 7), Vector3(0, 0, 0), Vector3(0, 1, 0));

	//renderMeshOnScreen.Init();
	wasEscPressed = false;
	isPause = false;

	fpsonce = false;
	playerDied = false;
	countDownbackToBase = 5.0;
	//camera = new Camera3;
	//camera->Init(Vector3(0, 0, 7), Vector3(0, 0, 0), Vector3(0, 1, 0));

	GoatBoss::getInstance();
	Player::getInstance()->CollisionMesh_->pos.Set(0, 0, 5);
	Player::addCollisionObject(GoatBoss::getInstance());
	Player::enemies_.push_back(GoatBoss::getInstance());
	deadBossBackToBaseTimer = 0.0;

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void BossScene::Update(double dt)
{


	int width, height;
	glfwGetWindowSize(Application::m_window, &width, &height);

	
	if (Application::IsKeyPressed('V') //&& fpsonce == false 
		|| !debugMode)
	{
		//delete camera;
		camera = FPSCam::getInstance();
//		fpsonce = true;
	}


	if (Player::getInstance()->getHp() <= 0)
	{
		playerDied = true;
		countDownbackToBase -= dt;
		if (countDownbackToBase <= 0.0)
		{
			GoatBoss::getInstance()->resetBoss();
			SceneManager::getInstance()->SetNextSceneID(SceneManager::SCENES::CAMPSCENE);
			SceneManager::getInstance()->SetNextScene();
		}
	}
	else
		Player::getInstance()->update(dt, camera);
	//for (size_t i = 0; i < CampNPC.size(); i++)
	//	CampNPC.at(i)->update(dt);
	GoatBoss::getInstance()->update(dt);



	double c_posx, c_posy;
	glfwGetCursorPos(Application::m_window, &c_posx, &c_posy);
	glfwSetCursorPos(Application::m_window, width / 2, height / 2);

	double dx, dy;
	dx = dt * double(width / 2 - c_posx);
	dy = dt * double(height / 2 - c_posy);
	camera->Update(dt, dx, dy);

	if (GoatBoss::getInstance()->Bhp_status == GoatBoss::getInstance()->BHP_DEAD)
	{
		//boss dead.
		deadBossBackToBaseTimer += dt;
		if (deadBossBackToBaseTimer >= 10)
		{
			GoatBoss::getInstance()->resetBoss();
			SceneManager::getInstance()->SetNextSceneID(SceneManager::SCENES::CAMPSCENE);
			SceneManager::getInstance()->SetNextScene();
		}
	}

	FramesPerSec = 1 / dt;
}

void BossScene::Render()
{
	// Render VBO here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera->getPosition().x, camera->getPosition().y, camera->getPosition().z,
		camera->getPosition().x + camera->getDir().x, camera->getPosition().y + camera->getDir().y, camera->getPosition().z + camera->getDir().z,
		camera->getUp().x, camera->getUp().y, camera->getUp().z);
	modelStack.LoadIdentity();

	RenderSkybox();
	RenderBaseCamp();
	//	renderEnvironment();

	//Ground Mesh


	//if (isPause)
	//	renderMeshOnScreen.renderPause(&projectionStack, &viewStack, &modelStack, m_parameters);



	GoatBoss::getInstance()->render(&projectionStack, &viewStack, &modelStack, m_parameters);
	Player::getInstance()->render(&projectionStack, &viewStack, &modelStack, m_parameters);

	if (playerDied)
	{
		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], "Oh Nos You Died!", Color(1, 0, 0), 2.5f, 32, 50, &projectionStack, &viewStack, &modelStack, m_parameters);
		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], "Back to Base in " + std::to_string((int)countDownbackToBase), Color(1, 0, 0), 2.f, 31, 47, &projectionStack, &viewStack, &modelStack, m_parameters);
	}
	RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::to_string(FramesPerSec), Color(1, 0, 0), 1.5f, 45, 38, &projectionStack, &viewStack, &modelStack, m_parameters);
}

void BossScene::RenderBaseCamp()
{



}

void BossScene::Exit()
{
	for (size_t i = 0; i < GEOMETRY_TYPE::NUM_GEOMETRY; i++)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	//delete camera;
	Player::getInstance()->clearCollisionObj();
	fpsonce = false;

	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
void BossScene::RenderSkybox()
{
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -skyBoxDistance);
	RenderMeshClass::RenderMesh(meshList[GEO_FRONT], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, skyBoxDistance);
	modelStack.Rotate(180.0f, 0, 1, 0);
	RenderMeshClass::RenderMesh(meshList[GEO_BACK], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-skyBoxDistance, 0, 0);
	modelStack.Rotate(90.0f, 0, 1, 0);
	RenderMeshClass::RenderMesh(meshList[GEO_LEFT], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(skyBoxDistance, 0, 0);
	modelStack.Rotate(-90.0f, 0, 1, 0);
	RenderMeshClass::RenderMesh(meshList[GEO_RIGHT], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, skyBoxDistance, 0);
	modelStack.Rotate(90.0f, 1, 0, 0);
	RenderMeshClass::RenderMesh(meshList[GEO_TOP], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, -skyBoxDistance, 0);
	modelStack.Rotate(-90.0f, 1, 0, 0);
	RenderMeshClass::RenderMesh(meshList[GEO_BOTTOM], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Rotate(-90.0f, 1, 0, 0);
	RenderMeshClass::RenderMesh(meshList[GROUND], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

}
