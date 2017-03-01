#include "InsideBarrackScene.h"
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

#include "FPSCam.h"
//#include "GameObject.h"

#include "NPC_Doc.h"
#include "Environment.h"

#include "RenderMesh.h"

#include "UI.h"

//InsideBarrackScene::Text_Data InsideBarrackScene::Text[TEXT_TYPE::Text_Count];
//unsigned InsideBarrackScene::m_parameters[U_TOTAL];

MS InsideBarrackScene::modelStack, InsideBarrackScene::viewStack, InsideBarrackScene::projectionStack;

//std::vector<GameObject*> InsideBarrackScene::Game_Objects_(10, NULL);

//std::vector<GameObject*> InsideBarrackScene::Game_Objects_(10, NULL);
//UI renderMeshOnScreen;

std::vector<EnvironmentObj*> InsideBarrackScene::Env_Obj;
Teleporter* InsideBarrackScene::Barrack;

//std::vector<NPC*> InsideBarrackScene::CampNPC;


InsideBarrackScene::InsideBarrackScene()
{
}

InsideBarrackScene::~InsideBarrackScene()
{
}

void InsideBarrackScene::Init()
{
	// Init VBO here
	Player::getInstance()->setPosition(Vector3(0, 0, 0));

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

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 1000, 1000, 1000);



	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("LightBall", Color(1, 1, 1), 12, 12, 1);
	meshList[GEO_LIGHTBALL1] = MeshBuilder::GenerateCylinder("LightBall1", Color(1, 1, 1), 12, 1, 0, 1);

	//	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere(...);
	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("image", Color(0.9f, 0.9f, 0.9f), 1.0f, 1.0f);
	//meshList[GEO_QUAD]->textureID = LoadTGA("Image//imagey.tga");
	meshList[GEO_QUAD]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_QUAD]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_QUAD]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_QUAD]->material.kShininess = 1.0f;


	//Ground Mesh ---- Red Dirt --------------- Start
	meshList[GEO_GroundMesh_RedDirt] = MeshBuilder::GenerateGround("Ground Mesh", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_GroundMesh_RedDirt]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GroundMesh_RedDirt]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_GroundMesh_RedDirt]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_GroundMesh_RedDirt]->material.kShininess = 1.0f;
	meshList[GEO_GroundMesh_RedDirt]->textureID = LoadTGA("Image//GroundMesh_RedDirt_Texture.tga");
	//Ground Mesh ---- Red Dirt ----------------- End




	//Walls, Door, Ceiling ------------------------------- START
	EnvironmentObj* Wall1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall1", "OBJ//InsideBarrackScene//Wall1_OBJ.obj"));
	Wall1->CollisionMesh_->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_Wall_UV_Texture.tga");
	EnvironmentObj* Wall2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall2", "OBJ//InsideBarrackScene//Wall2_OBJ.obj"));
	Wall2->CollisionMesh_->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_Wall_UV_Texture.tga");
	EnvironmentObj* Wall3 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall3", "OBJ//InsideBarrackScene//Wall3_OBJ.obj"));
	Wall3->CollisionMesh_->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_Wall_UV_Texture.tga");
	EnvironmentObj* Door = new EnvironmentObj(MeshBuilder::GenerateOBJ("Door1", "OBJ//InsideBarrackScene//Door1_OBJ.obj"));
	Door->CollisionMesh_->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_Door_UV_Texture.tga");

	//Barrack = new Teleporter(MeshBuilder::GenerateOBJ("Door1", "OBJ//InsideBarrackScene//Door1_OBJ.obj"), 0);
	//Barrack->CollisionMesh_->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_Door_UV_Texture.tga");
	
	EnvironmentObj* Ceiling1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Ceiling1", "OBJ//InsideBarrackScene//Ceiling1_OBJ.obj"));
	Ceiling1->CollisionMesh_->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_Door_UV_Texture.tga");

	//meshList[GEO_DOOR] = MeshBuilder::GenerateOBJ("Door1", "OBJ//InsideBarrackScene//Door1_OBJ.obj");
	//meshList[GEO_DOOR]->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_Door_UV_Texture.tga");

	Env_Obj.push_back(Wall1);
	Env_Obj.push_back(Wall2);
	Env_Obj.push_back(Wall3);
	Env_Obj.push_back(Ceiling1);
	Env_Obj.push_back(Door);

	//Player::getInstance()->Teleport.push_back(Barrack);
	//Walls, Door, Ceiling ------------------------------- END




	//Support Beams ------------------------------- START
	EnvironmentObj* Beam1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Beam1", "OBJ//InsideBarrackScene//SupportBeam1_OBJ.obj"));
	Beam1->CollisionMesh_->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_SupportingBeam_UV_Texture.tga");
	EnvironmentObj* Beam2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Beam1", "OBJ//InsideBarrackScene//SupportBeam2_OBJ.obj"));
	Beam2->CollisionMesh_->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_SupportingBeam_UV_Texture.tga");
	EnvironmentObj* Beam3 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Beam1", "OBJ//InsideBarrackScene//SupportBeam3_OBJ.obj"));
	Beam3->CollisionMesh_->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_SupportingBeam_UV_Texture.tga");
	EnvironmentObj* Beam4 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Beam1", "OBJ//InsideBarrackScene//SupportBeam4_OBJ.obj"));
	Beam4->CollisionMesh_->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_SupportingBeam_UV_Texture.tga");


	Env_Obj.push_back(Beam1);
	Env_Obj.push_back(Beam2);
	Env_Obj.push_back(Beam3);
	Env_Obj.push_back(Beam4);
	//Support Beams ------------------------------ END




	//Resting Area ------------------------------- START
	EnvironmentObj* Bed1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Bed1", "OBJ//InsideBarrackScene//Bed1_OBJ.obj"));
	Bed1->CollisionMesh_->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_SleepingStation_UV_Texture.tga");
	EnvironmentObj* Bed2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Bed2", "OBJ//InsideBarrackScene//Bed2_OBJ.obj"));
	Bed2->CollisionMesh_->textureID = LoadTGA("Image//InsideBarrackScene//InsideBarracks_SleepingStation_UV_Texture.tga");

	Env_Obj.push_back(Bed1);
	Env_Obj.push_back(Bed2);
	//Resting Area ------------------------------- END




	//Skybox ------------- Start
	//Left Skybox 
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("Left", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//sky1_left.tga");

	//Right Skybox 
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("Right", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//sky1_right.tga");

	//Front Skybox 
	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("Front", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//sky1_front.tga");

	//Back Skybox 
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("Back", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//sky1_back.tga");

	//Top Skybox 
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("Top", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//sky1_up.tga");

	//Bottom Skybox 
	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("Bottom", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//sky1_down.tga");
	//Skybox ------------- End



	for (auto it : Env_Obj)
		Player::addCollisionObject(it);


	//renderUI.Init();
	wasEscPressed = false;
	isPause = false;
	//MainMenu.Init();

	camera = FPSCam::getInstance();
	//camera->Init(Vector3(0, 0, 7), Vector3(0, 0, 0), Vector3(0, 1, 0));


	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void InsideBarrackScene::Update(double dt)
{

	int width, height;
	glfwGetWindowSize(Application::m_window, &width, &height);
	isEscPressed = Application::IsKeyPressed(VK_ESCAPE);


	if (Application::IsKeyPressed(VK_NUMPAD1) || Application::IsKeyPressed('1'))
	{
		glEnable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed(VK_NUMPAD2) || Application::IsKeyPressed('2'))
	{
		glDisable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed(VK_NUMPAD3) || Application::IsKeyPressed('3'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	}
	if (Application::IsKeyPressed(VK_NUMPAD4) || Application::IsKeyPressed('4'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	}

	//bool once = false;
	//if (Application::IsKeyPressed('1') && once == false)
	//{
	//	SceneManager::getInstance()->SetNextScene();
	//	//SceneManager::getInstance()->SetNextSceneID(0);
	//	once = true;
	//}

	//bool fpsonce = false;
	//if (Application::IsKeyPressed('V') && fpsonce == false)
	//{
	//	camera = FPSCam::getInstance();
	//	fpsonce = true;
	//}



	if (isEscPressed && !wasEscPressed) // When you press ESC
	{
		//if (!MainMenu.isMainMenu)
		//{
		if (!isPause)
		{
			isPause = true;
			glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPos(Application::m_window, width / 2, height / 2);
		}
		else
		{
			isPause = false;
			glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(Application::m_window, width / 2, height / 2);
		}

		wasEscPressed = isEscPressed;
		//}
	}

	if (!isEscPressed && wasEscPressed) // When you release the ESC button
		wasEscPressed = isEscPressed;

	Player::getInstance()->update(dt, camera);

	//for (size_t i = 0; i < CampNPC.size(); i++)
	//{
	//	CampNPC.at(i)->update(dt);
	//}


	if (!isPause /*&& !mainmenu.ismainmenu*/)
	{
		double c_posx, c_posy;
		glfwGetCursorPos(Application::m_window, &c_posx, &c_posy);
		glfwSetCursorPos(Application::m_window, width / 2, height / 2);

		glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		double dx, dy;
		dx = dt * double(width / 2 - c_posx);
		dy = dt * double(height / 2 - c_posy);
		camera->Update(dt, dx, dy);
	}

	FramesPerSec = 1 / dt;

	//MainMenu.Update(dt);
}

void InsideBarrackScene::Render()
{
	// Render VBO here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera->getPosition().x, camera->getPosition().y, camera->getPosition().z,
		camera->getPosition().x + camera->getDir().x, camera->getPosition().y + camera->getDir().y, camera->getPosition().z + camera->getDir().z,
		camera->getUp().x, camera->getUp().y, camera->getUp().z);
	modelStack.LoadIdentity();

	//if (MainMenu.isMainMenu)
	//	MainMenu.Render();

	//else
	//{

	RenderSkybox();
	//renderEnvironment();

	//Ground Mesh
	modelStack.PushMatrix();
	modelStack.Scale(100, 100, 100);
	modelStack.Rotate(90, -1, 0, 0);
	RenderMeshClass::RenderMesh(meshList[GEO_GroundMesh_RedDirt], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();



	//for (size_t i = 0; i < CampNPC.size(); i++)
	//{
	//	CampNPC.at(i)->render(&projectionStack, &viewStack, &modelStack, m_parameters);
	//}



	//if (isPause)
	//renderUI.renderPause(&projectionStack, &viewStack, &modelStack, m_parameters);


	//}

	for (size_t i = 0; i < Env_Obj.size(); i++)
	{
		modelStack.PushMatrix();
		RenderMeshClass::RenderMesh(Env_Obj.at(i)->CollisionMesh_, true, &projectionStack, &viewStack, &modelStack, m_parameters);
		modelStack.PopMatrix();

	}


	Interactions();

	RenderMeshClass::RenderMesh(meshList[GEO_AXES], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	Player::getInstance()->render(&projectionStack, &viewStack, &modelStack, m_parameters);

	RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::to_string(FramesPerSec), Color(1, 0, 0), 1.5f, 45, 38, &projectionStack, &viewStack, &modelStack, m_parameters);


}

void InsideBarrackScene::Interactions(){

	if (Player::getInstance()->getPlayerPosition().x >= -1 && Player::getInstance()->getPlayerPosition().x <= 1 && Player::getInstance()->getPlayerPosition().z <= -1.5 && Player::getInstance()->getPlayerPosition().z >= -2){

		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to exit.]"), Color(1, 0, 0), 2.f, 35, 24, &projectionStack, &viewStack, &modelStack, m_parameters);

		if (Application::IsKeyPressed(VK_SPACE)){

			SceneManager::getInstance()->SetNextSceneID(SceneManager::SCENES::CAMPSCENE);
			SceneManager::getInstance()->SetNextScene();

			Player::getInstance()->setPosition(Vector3(-6.5, 0.0, -14.0));
		}
	}

}

void InsideBarrackScene::Exit()
{
	for (size_t i = 0; i < GEOMETRY_TYPE::NUM_GEOMETRY; i++)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	//delete camera;
	Player::getInstance()->clearCollisionObj();

	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void InsideBarrackScene::RenderSkybox()
{
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -248);
	modelStack.Scale(500.0f, 500.0f, 500.0f);
	RenderMeshClass::RenderMesh(meshList[GEO_FRONT], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 248);
	modelStack.Rotate(180.0f, 0, 1, 0);
	modelStack.Scale(500.0f, 500.0f, 500.0f);
	RenderMeshClass::RenderMesh(meshList[GEO_BACK], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-248, 0, 0);
	modelStack.Rotate(90.0f, 0, 1, 0);
	modelStack.Scale(500.0f, 500.0f, 500.0f);
	RenderMeshClass::RenderMesh(meshList[GEO_LEFT], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(248, 0, 0);
	modelStack.Rotate(-90.0f, 0, 1, 0);
	modelStack.Scale(500.0f, 500.0f, 500.0f);
	RenderMeshClass::RenderMesh(meshList[GEO_RIGHT], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 248, 0);
	modelStack.Rotate(90.0f, 0, 1, 0);
	modelStack.Rotate(90.0f, 1, 0, 0);
	modelStack.Scale(500.0f, 500.0f, 500.0f);
	RenderMeshClass::RenderMesh(meshList[GEO_TOP], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, -248, 0);
	modelStack.Rotate(90.0f, 0, 1, 0);
	modelStack.Rotate(-90.0f, 1, 0, 0);
	modelStack.Scale(500.0f, 500.0f, 500.0f);
	RenderMeshClass::RenderMesh(meshList[GEO_BOTTOM], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();


}