#include "MainScene.h"
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
#include "Teleporter.h"
#include "Inventory.h"

#include "RenderMesh.h"

#include "UI.h"
#include "Blueprints.h"
#include "Item.h"

//MainScene::Text_Data MainScene::Text[TEXT_TYPE::Text_Count];
//unsigned MainScene::m_parameters[U_TOTAL];
MS MainScene::modelStack, MainScene::viewStack, MainScene::projectionStack;

//std::vector<GameObject*> MainScene::Game_Objects_(10, NULL);
std::vector<Item*> MainScene::Item_Obj;
std::vector<EnvironmentObj*> MainScene::Env_Obj;
std::vector<NPC*> MainScene::CampNPC;
Teleporter* MainScene::Barrack;


MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

void MainScene::Init()
{
	// Init VBO here
	Player::getInstance()->setPosition(Vector3(12, 0, 11));

	glClearColor(0.0f, 0.5f, 0.66f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Load vertex and fragment shaders
	//m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Blending.fragmentshader");


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



	//Ground Mesh ---- Red Dirt --------------- Start
	meshList[GEO_GroundMesh_RedDirt] = MeshBuilder::GenerateGround("Ground Mesh", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_GroundMesh_RedDirt]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GroundMesh_RedDirt]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_GroundMesh_RedDirt]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_GroundMesh_RedDirt]->material.kShininess = 1.0f;
	meshList[GEO_GroundMesh_RedDirt]->textureID = LoadTGA("Image//GroundMesh_RedDirt_Texture.tga");
	//Ground Mesh ---- Red Dirt ----------------- End

	//textureList[test] = MeshBuilder::GenerateQuad("Test", Color(1, 1, 1), 1, 1);
	//textureList[test]->textureID = LoadTGA("Image//inventory.tga");
	Inventory::getInstance()->Init();
	a = new Item("a");
	a->CollisionMesh_ = MeshBuilder::GenerateOBJ("Testing", "OBJ//testing.obj");
	a->CollisionMesh_->textureID = LoadTGA("Image//testing.tga");
	a->item2DTexture = MeshBuilder::GenerateQuad("quad", Color(1, 1, 1), 1.f, 1.f);
	a->item2DTexture->textureID = LoadTGA("Image//inventory.tga");
	Player::getInstance()->Items.push_back(a);

	b = new Item("b");
	b->CollisionMesh_ = MeshBuilder::GenerateOBJ("Testing", "OBJ//goat.obj");
	b->CollisionMesh_->textureID = LoadTGA("Image//testing.tga");
	b->item2DTexture = MeshBuilder::GenerateQuad("quad", Color(1, 1, 1), 1.f, 1.f);
	b->item2DTexture->textureID = LoadTGA("Image//CrossHair.tga");
	Player::getInstance()->Items.push_back(b);

	Item_Obj.push_back(a);
	Item_Obj.push_back(b);
	//Inventory::getInstance()->setItem(a);

	//Skybox ------------ Base Camp Start
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
	//Skybox ------------- Base Camp End




	//Barricade -------------------- Start
	EnvironmentObj* Barricade1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Barricade", "OBJ//Barricade1_OBJ.obj"));
	Barricade1->CollisionMesh_->textureID = LoadTGA("Image//Barricade_UV_Texture.tga");
	EnvironmentObj* Barricade2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Barricade", "OBJ//Barricade2_OBJ.obj"));
	Barricade2->CollisionMesh_->textureID = LoadTGA("Image//Barricade_UV_Texture.tga");
	EnvironmentObj* Barricade3 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Barricade", "OBJ//Barricade3_OBJ.obj"));
	Barricade3->CollisionMesh_->textureID = LoadTGA("Image//Barricade_UV_Texture.tga");
	EnvironmentObj* Barricade4 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Barricade", "OBJ//Barricade4_OBJ.obj"));
	Barricade4->CollisionMesh_->textureID = LoadTGA("Image//Barricade_UV_Texture.tga");

	Env_Obj.push_back(Barricade1);
	Env_Obj.push_back(Barricade2);
	Env_Obj.push_back(Barricade3);
	Env_Obj.push_back(Barricade4);
	//Barricade -------------------- End




	//Teleporter ------------------ START
	//Teleporter* MS_Teleporter = new Teleporter(MeshBuilder::GenerateOBJ("Boss", "OBJ//Teleporter1_OBJ.obj"), SceneManager::SCENES::BOSSSCENE);
	//MS_Teleporter->CollisionMesh_->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");
	//Env_Obj.push_back(MS_Teleporter);


	Teleporter* To_WS_Teleporter = new Teleporter(MeshBuilder::GenerateOBJ("City", "OBJ//Teleporter_OBJ.obj"), SceneManager::SCENES::WORLDSCENE);
	To_WS_Teleporter->CollisionMesh_->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");

	//Teleporter1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Teleporter1", "OBJ//Teleporter1_OBJ.obj"));
	//Teleporter1->CollisionMesh_->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");

	EnvironmentObj* MS_Teleporter = new EnvironmentObj(MeshBuilder::GenerateOBJ("Collect 3 Blueprints to enter", "OBJ//Teleporter1_OBJ.obj"));
	MS_Teleporter->CollisionMesh_->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");

	meshList[GEO_Teleporter] = MeshBuilder::GenerateOBJ("Teleporter to WS", "OBJ//Teleporter_OBJ.obj");
	meshList[GEO_Teleporter]->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");
	
	//meshList[GEO_Teleporter1] = MeshBuilder::GenerateOBJ("Teleporter to BS", "OBJ//Teleporter1_OBJ.obj");
	//meshList[GEO_Teleporter1]->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");

	if (Blueprints::GetBlueprintNumber() == 3){

		Teleporter* MS_Teleporter = new Teleporter(MeshBuilder::GenerateOBJ("Boss", "OBJ//Teleporter1_OBJ.obj"), SceneManager::SCENES::BOSSSCENE);
		MS_Teleporter->CollisionMesh_->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");

		Player::getInstance()->teleporters_.push_back(MS_Teleporter);
		Player::getInstance()->addCollisionObject(MS_Teleporter);
	}

	Env_Obj.push_back(MS_Teleporter);

	Player::getInstance()->teleporters_.push_back(To_WS_Teleporter);
	Player::getInstance()->addCollisionObject(To_WS_Teleporter);

	//Teleporter -----------------------END




	NPC* Doc = new NPC_DOC();
	//Game_Objects_.push_back(Doc);
	CampNPC.push_back(Doc);




	//Medical Tents ------------------- START
	EnvironmentObj* MedicalTent1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Medical Tent1", "OBJ//MedicalTent1_OBJ.obj"));
	MedicalTent1->CollisionMesh_->textureID = LoadTGA("Image//MedicalCamp_UV_Texture.tga");
	EnvironmentObj* MedicalTent2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Medical Tent2", "OBJ//MedicalTent2_OBJ.obj"));
	MedicalTent2->CollisionMesh_->textureID = LoadTGA("Image//MedicalCamp_UV_Texture.tga");
	EnvironmentObj* MedicalTent3 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Medical Tent3", "OBJ//MedicalTent3_OBJ.obj"));
	MedicalTent3->CollisionMesh_->textureID = LoadTGA("Image//MedicalCamp_UV_Texture.tga");
	EnvironmentObj* MedicalTent4 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Medical Tent4", "OBJ//MedicalTent4_OBJ.obj"));
	MedicalTent4->CollisionMesh_->textureID = LoadTGA("Image//MedicalCamp_UV_Texture.tga");

	Env_Obj.push_back(MedicalTent1);
	Env_Obj.push_back(MedicalTent2);
	Env_Obj.push_back(MedicalTent3);
	Env_Obj.push_back(MedicalTent4);
	//Medical Tent ------------------- END




	//Barracks -------------------- START
	EnvironmentObj* Barrack1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Barrack1", "OBJ//Barracks1_OBJ.obj"));
	Barrack1->CollisionMesh_->textureID = LoadTGA("Image//Barracks_UV_Texture.tga");
	EnvironmentObj* Barrack2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Barrack2", "OBJ//Barracks2_OBJ.obj"));
	Barrack2->CollisionMesh_->textureID = LoadTGA("Image//Barracks_UV_Texture.tga");

	//Barrack = new Teleporter(MeshBuilder::GenerateOBJ("Barrack2", "OBJ//Barracks2_OBJ.obj"),2);
	//Barrack->CollisionMesh_->textureID = LoadTGA("Image//Barracks_UV_Texture.tga");

	EnvironmentObj* Barrack3 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Barrack3", "OBJ//Barracks3_OBJ.obj"));
	Barrack3->CollisionMesh_->textureID = LoadTGA("Image//Barracks_UV_Texture.tga");
	EnvironmentObj* Barrack4 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Barrack4", "OBJ//Barracks4_OBJ.obj"));
	Barrack4->CollisionMesh_->textureID = LoadTGA("Image//Barracks_UV_Texture.tga");

	//meshList[GEO_Barrack] = MeshBuilder::GenerateOBJ("Barrack2", "OBJ//Barracks2_OBJ.obj");
	//meshList[GEO_Barrack]->textureID = LoadTGA("Image//Barracks_UV_Texture.tga");

	Env_Obj.push_back(Barrack1);
	Env_Obj.push_back(Barrack2);
	Env_Obj.push_back(Barrack3);
	Env_Obj.push_back(Barrack4);

	//Player::getInstance()->Teleport.push_back(Barrack);
	//Barracks ------------------------- END




	//Crates ------------------------------- START
	meshList[GEO_Crates1] = MeshBuilder::GenerateOBJ("Barracks", "OBJ//Crates1_OBJ.obj");
	meshList[GEO_Crates1]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_Crates1]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_Crates1]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_Crates1]->material.kShininess = 1.0f;
	meshList[GEO_Crates1]->textureID = LoadTGA("Image//Crate_UV_Texture.tga");

	EnvironmentObj* Crate = new EnvironmentObj(MeshBuilder::GenerateOBJ("Crates", "OBJ//Crates_OBJ.obj"));
	Crate->CollisionMesh_->textureID = LoadTGA("Image//Crate_UV_Texture.tga");
	EnvironmentObj* Crate1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Crate1", "OBJ//Crates1_OBJ.obj"));
	Crate1->CollisionMesh_->textureID = LoadTGA("Image//Crate_UV_Texture.tga");

	Env_Obj.push_back(Crate);
	Env_Obj.push_back(Crate1);
	//Crates ------------------------------------------- END




	//Solar Panels------------------------------------- START
	EnvironmentObj* SolarPanel1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Solar Panel1", "OBJ//SolarPanel1_OBJ.obj"));
	SolarPanel1->CollisionMesh_->textureID = LoadTGA("Image//SolarUV_Texture.tga");
	EnvironmentObj* SolarPanel2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Solar Panel2", "OBJ//SolarPanel2_OBJ.obj"));
	SolarPanel2->CollisionMesh_->textureID = LoadTGA("Image//SolarUV_Texture.tga");
	EnvironmentObj* SolarPanel3 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Solar Panel3", "OBJ//SolarPanel3_OBJ.obj"));
	SolarPanel3->CollisionMesh_->textureID = LoadTGA("Image//SolarUV_Texture.tga");
	EnvironmentObj* SolarPanel4 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Solar Panel4", "OBJ//SolarPanel4_OBJ.obj"));
	SolarPanel4->CollisionMesh_->textureID = LoadTGA("Image//SolarUV_Texture.tga");

	Env_Obj.push_back(SolarPanel1);
	Env_Obj.push_back(SolarPanel2);
	Env_Obj.push_back(SolarPanel3);
	Env_Obj.push_back(SolarPanel4);
	//Solar Panels ------------------------------------- END




	//Powerbox ----------------------------------- START
	EnvironmentObj* PowerBox = new EnvironmentObj(MeshBuilder::GenerateOBJ("PB", "OBJ//Powerbox_OBJ.obj"));
	PowerBox->CollisionMesh_->textureID = LoadTGA("Image//Powerbox_UV_Texture.tga");

	Env_Obj.push_back(PowerBox);
	//Powerbox ----------------------------------- END



	//Captain ------------------------------------------ START
	EnvironmentObj* Captain = new EnvironmentObj(MeshBuilder::GenerateOBJ("Captain", "OBJ//Captain_OBJ.obj"));
	Captain->CollisionMesh_->textureID = LoadTGA("Image//GuardUV.tga");

	Env_Obj.push_back(Captain);
	//Captain ----------------------------------------- END





	InitGuards();

	for (auto it : Env_Obj)
		Player::addCollisionObject(it);

	for (auto it : Item_Obj)
		Player::Items.push_back(it);

	for (auto it : CampNPC)
		Player::NPCs.push_back(it);

	Player::getInstance()->setHpToMax();
	UI::getInstance()->Init();


	camera = new Camera3;
	camera->Init(Vector3(0, 0, 7), Vector3(0, 0, 0), Vector3(0, 1, 0));

	wantPlay = false;
	talktoGuard = false;

	meshList[WANTPLAY] = MeshBuilder::GenerateQuad("want play", Color(1.f, 1.f, 1.f), 1.f, 1.f);
	meshList[WANTPLAY]->textureID = LoadTGA("Image//wantplay.tga");

	meshList[YES] = MeshBuilder::GenerateQuad("yes", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	meshList[YES]->textureID = LoadTGA("Image//yes.tga");
	meshList[YES]->setHb(true, Vector3(-100.f, -50.f, 0.f), Vector3(100.f, 50.f, 2.f),
		Vector3((float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 768) * (Application::getWindowHeight() - 400), 0.f),
		Vector3(0.f, 1.f, 0.f));

	meshList[YES2] = MeshBuilder::GenerateQuad("yes", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	meshList[YES2]->textureID = LoadTGA("Image//yes2.tga");

	meshList[NO] = MeshBuilder::GenerateQuad("no", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	meshList[NO]->textureID = LoadTGA("Image//no.tga");
	meshList[NO]->setHb(true, Vector3(-100.f, -50.f, 0.f), Vector3(100.f, 50.f, 2.f),
		Vector3((float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 768) * (Application::getWindowHeight() - 200), 0.f),
		Vector3(0.f, 1.f, 0.f));

	meshList[NO2] = MeshBuilder::GenerateQuad("no", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	meshList[NO2]->textureID = LoadTGA("Image//no2.tga");

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void MainScene::Update(double dt)
{
	glfwGetWindowSize(Application::m_window, &width, &height);



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

	//NEEEDS TO CHANGE SOOOONNN
	//bool once = false;
	//if (Application::IsKeyPressed('1') && once == false)
	//{
	//	SceneManager::getInstance()->SetNextSceneID(SceneManager::SCENES::MINIGAMESCENE);
	//	SceneManager::getInstance()->SetNextScene();
	//	once = true;
	//}


	//bool fpsonce = false;
	if (Application::IsKeyPressed('V') || !debugMode)
	{
		//delete camera;
		//camera = NULL;
		camera = FPSCam::getInstance();
		//fpsonce = true;
	}

	Player::getInstance()->update(dt, camera);
	Inventory::getInstance()->Update(dt);
	UI::getInstance()->Update(dt);




	if (!UI::getInstance()->isPauseOpen() && !Inventory::getInstance()->isInventoryOpen())
	{
		
		glfwGetCursorPos(Application::m_window, &c_posx, &c_posy);

		if (!wantPlay)
		{
			glfwSetCursorPos(Application::m_window, width / 2, height / 2);
			dx, dy;
			dx = dt * double(width / 2 - c_posx);
			dy = dt * double(height / 2 - c_posy);

			camera->Update(dt, dx, dy);
			glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		for (size_t i = 0; i < CampNPC.size(); i++)
		{
			CampNPC.at(i)->update(dt);
		}
	}

	if (Player::getInstance()->getPlayerPosition().x <= 3.f && Player::getInstance()->getPlayerPosition().x >= 1.f && Player::getInstance()->getPlayerPosition().z >= -9.f && Player::getInstance()->getPlayerPosition().z <= -6.f)
		talktoGuard = true;

	else
		talktoGuard = false;

	if (talktoGuard)
	{
		if (Application::IsKeyPressed(VK_SPACE))
		{
			wantPlay = true;
			glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	if (wantPlay)
	{
		if (meshList[YES]->isPointInsideAABB(Position((float)c_posx, (float)c_posy, 0.f)))
		{
			yes = true;

			if (glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				SceneManager::getInstance()->SetNextSceneID(SceneManager::SCENES::MINIGAMESCENE);
				SceneManager::getInstance()->SetNextScene();
			}

		}

		else
			yes = false;

		if (meshList[NO]->isPointInsideAABB(Position((float)c_posx, (float)c_posy, 0.f)))
		{
			no = true;

			if (glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				wantPlay = false;
				glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}

		}

		else
			no = false;
	}


	FramesPerSec = 1 / dt;
}

void MainScene::Render()
{
	// Render VBO here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera->getPosition().x, camera->getPosition().y, camera->getPosition().z,
		camera->getPosition().x + camera->getDir().x, camera->getPosition().y + camera->getDir().y, camera->getPosition().z + camera->getDir().z,
		camera->getUp().x, camera->getUp().y, camera->getUp().z);
	modelStack.LoadIdentity();


	RenderMeshClass::RenderMesh(meshList[GEO_AXES], false, &projectionStack, &viewStack, &modelStack, m_parameters);


	//for (size_t i = 0; i < Item_Obj.size(); i++)
	//{
	//	if (!Player::Items[i]->isItemInInventory)
	//	{
	//		modelStack.PushMatrix();
	//		RenderMeshClass::RenderMesh(Item_Obj.at(i)->CollisionMesh_, true, &projectionStack, &viewStack, &modelStack, m_parameters);
	//		modelStack.PopMatrix();
	//	}
	//}

	RenderSkybox();
	robotsInteractions();
	//	renderEnvironment();


	//Ground Mesh
	modelStack.PushMatrix();
	modelStack.Scale(1000, 1000, 1000);
	modelStack.Rotate(90, -1, 0, 0);
	RenderMeshClass::RenderMesh(meshList[GEO_GroundMesh_RedDirt], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	for (size_t i = 0; i < CampNPC.size(); i++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(7, 0, 0);
		CampNPC.at(i)->render(&projectionStack, &viewStack, &modelStack, m_parameters);
		modelStack.PopMatrix();
	}


	RenderBaseCamp();

	modelStack.PushMatrix();
	RenderMeshClass::RenderMesh(meshList[GEO_Teleporter], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	//RenderMeshClass::RenderMesh(meshList[GEO_Teleporter1], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	robotsInteractions();
	medicInteractions();

	Interactions();
	
	modelStack.PushMatrix();
	modelStack.Translate(11, 2, 5);
	modelStack.Scale(0.3f, 0.3f, 0.3f);
	if (counter_medic == 0){
	
		RenderMeshClass::RenderText(&Scene::Text[Scene::TEXT_TYPE::Century], std::string("Talk to Doctor!"), Color(0, 1, 0), &projectionStack, &viewStack, &modelStack, m_parameters);
	}
	modelStack.PopMatrix();


	RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::to_string(FramesPerSec), Color(0, 1, 0), 1.5f, 45, 38, &projectionStack, &viewStack, &modelStack, m_parameters);
	RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Blueprints: ") + std::to_string(Blueprints::GetBlueprintNumber()) + std::string("/3"), Color(0, 1, 0), 2.f, 68, 57, &projectionStack, &viewStack, &modelStack, m_parameters);
	
	Player::getInstance()->render(&projectionStack, &viewStack, &modelStack, m_parameters);

	UI::getInstance()->renderPause(&projectionStack, &viewStack, &modelStack, m_parameters);
	Inventory::getInstance()->Render(&projectionStack, &viewStack, &modelStack, m_parameters);
	
	if (talktoGuard)
		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], "[Press SPACE to talk to Guard 5.]", Color(0, 1, 0), 2.f, 35, 24, &projectionStack, &viewStack, &modelStack, m_parameters);

	if (wantPlay)
	{
		RenderMeshClass::RenderMeshOnScreen(meshList[WANTPLAY], Application::getWindowWidth() * 0.5f, Application::getWindowHeight() * 0.5f, 0, 700, 700, &projectionStack, &viewStack, &modelStack, m_parameters);

		if (!yes)
			RenderMeshClass::RenderMeshOnScreen(meshList[YES], (int)Application::getWindowWidth() * 0.5f, (int)(Application::getWindowHeight() / 768) * 400, 0, (int)(Application::getWindowWidth() / 1024) * 200, (int)(Application::getWindowHeight() / 768) * 100, &projectionStack, &viewStack, &modelStack, m_parameters);

		else
			RenderMeshClass::RenderMeshOnScreen(meshList[YES2], (int)Application::getWindowWidth() * 0.5f, (int)(Application::getWindowHeight() / 768) * 400, 0, (int)(Application::getWindowWidth() / 1024) * 200, (int)(Application::getWindowHeight() / 768) * 100, &projectionStack, &viewStack, &modelStack, m_parameters);

		if (!no)
			RenderMeshClass::RenderMeshOnScreen(meshList[NO], (int)Application::getWindowWidth() * 0.5f, (int)(Application::getWindowHeight() / 768) * 200.f, 0, (int)(Application::getWindowWidth() / 1024) * 200, (int)(Application::getWindowHeight() / 768) * 100, &projectionStack, &viewStack, &modelStack, m_parameters);

		else
			RenderMeshClass::RenderMeshOnScreen(meshList[NO2], (int)Application::getWindowWidth() * 0.5f, (int)(Application::getWindowHeight() / 768) * 200.f, 0, (int)(Application::getWindowWidth() / 1024) * 200, (int)(Application::getWindowHeight() / 768) * 100, &projectionStack, &viewStack, &modelStack, m_parameters);
	}
}

void MainScene::Interactions(){

	if (Player::getInstance()->getPlayerPosition().x >= -8 && Player::getInstance()->getPlayerPosition().x <= -5 && Player::getInstance()->getPlayerPosition().z <= -11 && Player::getInstance()->getPlayerPosition().z >= -17){

		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to enter.]"), Color(0, 1, 0), 2.f, 35, 24, &projectionStack, &viewStack, &modelStack, m_parameters);
	}

	if (Application::IsKeyPressed(VK_SPACE)){

		if (Player::getInstance()->getPlayerPosition().x >= -8 && Player::getInstance()->getPlayerPosition().x <= -5 && Player::getInstance()->getPlayerPosition().z <= -11 && Player::getInstance()->getPlayerPosition().z >= -17){

			SceneManager::getInstance()->SetNextSceneID(SceneManager::SCENES::BARRACKSCENE);
			SceneManager::getInstance()->SetNextScene();
			return;
		}
	}
}

void MainScene::RenderBaseCamp(){

	for (size_t i = 0; i < Env_Obj.size(); i++)
	{
		modelStack.PushMatrix();
		RenderMeshClass::RenderMesh(Env_Obj.at(i)->CollisionMesh_, true, &projectionStack, &viewStack, &modelStack, m_parameters);
		modelStack.PopMatrix();

	}
}

void MainScene::Exit()
{
	for (size_t i = 0; i < GEOMETRY_TYPE::NUM_GEOMETRY; i++)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	//delete camera;
	Player::getInstance()->clearCollisionObj();
	delete Barrack;

	while (Env_Obj.size())
		Env_Obj.pop_back();
	while (CampNPC.size())
		CampNPC.pop_back();

	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
void MainScene::RenderSkybox()
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

void MainScene::robotsInteractions(){


	if (Player::getInstance()->getPlayerPosition().x >= -12 && Player::getInstance()->getPlayerPosition().x <= -9 && Player::getInstance()->getPlayerPosition().z <= -0.5 && Player::getInstance()->getPlayerPosition().z >= -4){

		if (counter == 0){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to talk.]"), Color(0, 1, 0), 2.f, 35, 24, &projectionStack, &viewStack, &modelStack, m_parameters);
		}
	}

	if (Application::IsKeyPressed(VK_SPACE)){

		counter = 1;
	}

	if (counter == 1){

		if (Player::getInstance()->getPlayerPosition().x >= -12 && Player::getInstance()->getPlayerPosition().x <= -9 && Player::getInstance()->getPlayerPosition().z <= -0.5 && Player::getInstance()->getPlayerPosition().z >= -4){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Ah, Commander! Good to see you up and about."), Color(0, 1, 0), 2.f, 10, 36, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Well, some of my scouts have reported several things."), Color(0, 1, 0), 2.f, 10, 34, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("I've got information at several different coordinates. Three, in fact."), Color(0, 1, 0), 2.f, 10, 32, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press 1 / 2 / 3 and hold for the information.]"), Color(0, 1, 0), 2.f, 10, 30, &projectionStack, &viewStack, &modelStack, m_parameters);
		}
	}


	if (Application::IsKeyPressed('1')){

		counter = 2;

		if (Player::getInstance()->getPlayerPosition().x >= -12 && Player::getInstance()->getPlayerPosition().x <= -9 && Player::getInstance()->getPlayerPosition().z <= -0.5 && Player::getInstance()->getPlayerPosition().z >= -4){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("There's a half destroyed robot, around coordinates X:75 Z:74."), Color(0, 1, 0), 2.f, 10, 36, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("It's heavily guarded, for some reason. Might have something you're looking for."), Color(0, 1, 0), 2.f, 10, 34, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Might have something you're looking for."), Color(0, 1, 0), 2.f, 10, 32, &projectionStack, &viewStack, &modelStack, m_parameters);
		}

	}

	if (Application::IsKeyPressed('2')){

		counter = 2;

		if (Player::getInstance()->getPlayerPosition().x >= -12 && Player::getInstance()->getPlayerPosition().x <= -9 && Player::getInstance()->getPlayerPosition().z <= -0.5 && Player::getInstance()->getPlayerPosition().z >= -4){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("There's a functioning computer, around coordinates X:-59 Z:39."), Color(0, 1, 0), 2.f, 10, 36, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Its stuck in a wall, though, so the scout couldn't bring it back."), Color(0, 1, 0), 2.f, 10, 34, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("You might want to check it out."), Color(0, 1, 0), 2.f, 10, 32, &projectionStack, &viewStack, &modelStack, m_parameters);
		}

	}

	if (Application::IsKeyPressed('3')){

		counter = 2;

		if (Player::getInstance()->getPlayerPosition().x >= -12 && Player::getInstance()->getPlayerPosition().x <= -9 && Player::getInstance()->getPlayerPosition().z <= -0.5 && Player::getInstance()->getPlayerPosition().z >= -4){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("There's something around the oil barrels, around coordinates X:-110 Z:90."), Color(0, 1, 0), 2.f, 10, 36, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("You might want to check it out."), Color(0, 1, 0), 2.f, 10, 34, &projectionStack, &viewStack, &modelStack, m_parameters);
		}

	}


	if (counter == 2){
		if (Player::getInstance()->getPlayerPosition().x >= -12 && Player::getInstance()->getPlayerPosition().x <= -9 && Player::getInstance()->getPlayerPosition().z <= -0.5 && Player::getInstance()->getPlayerPosition().z >= -4){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press C to go back]"), Color(0, 1, 0), 2.f, 10, 38, &projectionStack, &viewStack, &modelStack, m_parameters);
		}
	}
	if (Application::IsKeyPressed('C')){

		counter = 1;
	}
}

void MainScene::InitGuards(){

	//Guards --------------------------------------------- START
	EnvironmentObj* Guard1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Guard1", "OBJ//Guard1_OBJ.obj"));
	Guard1->CollisionMesh_->textureID = LoadTGA("Image//GuardUV.tga");
	EnvironmentObj* Guard2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Guard2", "OBJ//Guard2_OBJ.obj"));
	Guard2->CollisionMesh_->textureID = LoadTGA("Image//GuardUV.tga");
	EnvironmentObj* Guard3 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Guard3", "OBJ//Guard3_OBJ.obj"));
	Guard3->CollisionMesh_->textureID = LoadTGA("Image//GuardUV.tga");
	EnvironmentObj* Guard4 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Guard4", "OBJ//Guard4_OBJ.obj"));
	Guard4->CollisionMesh_->textureID = LoadTGA("Image//GuardUV.tga");
	EnvironmentObj* Guard5 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Guard5", "OBJ//Guard5_OBJ.obj"));
	Guard5->CollisionMesh_->textureID = LoadTGA("Image//GuardUV.tga");

	Env_Obj.push_back(Guard1);
	Env_Obj.push_back(Guard2);
	Env_Obj.push_back(Guard3);
	Env_Obj.push_back(Guard4);
	Env_Obj.push_back(Guard5);

	//Guards ---------------------------------------------- END

}

void MainScene::medicInteractions(){
		
	RenderMeshClass::RenderText;

	if (Player::getInstance()->getPlayerPosition().x >= 10 && Player::getInstance()->getPlayerPosition().x <= 14 && Player::getInstance()->getPlayerPosition().z <= 10 && Player::getInstance()->getPlayerPosition().z >= 5){
	
		if (counter_medic == 0){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Ah, Commander! I see that you have finished booting up!"), Color(0, 1, 0), 2.f, 15, 36, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("You might be a little disoriented, but you should be fine."), Color(0, 1, 0), 2.f, 15, 34, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press F to continue]"), Color(0, 1, 0), 2.f, 15, 32, &projectionStack, &viewStack, &modelStack, m_parameters);
		}
	}

	if (Application::IsKeyPressed('F')){

		counter_medic = 1;
	}

	if (counter_medic == 1){

		if (Player::getInstance()->getPlayerPosition().x >= 10 && Player::getInstance()->getPlayerPosition().x <= 14 && Player::getInstance()->getPlayerPosition().z <= 10 && Player::getInstance()->getPlayerPosition().z >= 5){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Excellent! You seem to be just fine!"), Color(0, 1, 0), 2.f, 10, 38, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Now, then, you should get going to see the Captain, that one guy with a hat."), Color(0, 1, 0), 2.f, 10, 36, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("He should have some information that could help you in your search!"), Color(0, 1, 0), 2.f, 10, 34, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Use W, A, S, D keys to move]"), Color(0, 1, 0), 2.f, 10, 32, &projectionStack, &viewStack, &modelStack, m_parameters);

		}
	}
}

//int MainScene::GetBlueprintNumber()
//{
//	return numberOfBlueprints;
//}
//
//void MainScene::SetBlueprintNumber(int new_number)
//{
//	numberOfBlueprints = new_number;
//}