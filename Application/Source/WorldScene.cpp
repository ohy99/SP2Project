#include "WorldScene.h"
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
#include "Blueprints.h"
#include "Inventory.h"

#include "MinionAI.h"

//WorldScene::Text_Data WorldScene::Text[TEXT_TYPE::Text_Count];
//unsigned WorldScene::m_parameters[U_TOTAL];
MS WorldScene::modelStack, WorldScene::viewStack, WorldScene::projectionStack;
MinionAI* WorldScene::goatMinionPool[5];

//std::vector<GameObject*> WorldScene::Game_Objects_(10, NULL);

//std::vector<GameObject*> WorldScene::Game_Objects_(10, NULL);
//UI renderMeshOnScreen;
std::vector<EnvironmentObj*> WorldScene::Env_Obj;
std::vector<MinionAI*>WorldScene::Enemy_;
Teleporter* WorldScene::WS_Teleporter;
Teleporter* WorldScene::Underground_Door;

//std::vector<NPC*> WorldScene::CampNPC;




WorldScene::WorldScene()
{
}

WorldScene::~WorldScene()
{
}

void WorldScene::Init()
{
	// Init VBO here
	//Player::getInstance()->setPosition(Vector3(0, 0, 0));

	glClearColor(0.0f, 0.5f, 0.66f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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


	initEnvironment();
	initItems();
	initblueprints();
	initSkybox();
	initEnemies();


	for (auto it : Env_Obj)
		Player::addCollisionObject(it);
	

	//renderUI.Init();
	wasEscPressed = false;
	isPause = false;
	//MainMenu.Init();

	UI::getInstance()->Init();

	camera = new Camera3;
	camera->Init(Vector3(0, 0, 7), Vector3(0, 0, 0), Vector3(0, 1, 0));



	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void WorldScene::Update(double dt)
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

	bool fpsonce = false;
	if (Application::IsKeyPressed('V') && fpsonce == false)
	{
		camera = FPSCam::getInstance();
		fpsonce = true;
	}

	for (int i = 0; i < Enemy_.size();i++)
	{
		Enemy_[i]->update(dt);
	}
	Player::getInstance()->update(dt, camera);
	Inventory::getInstance()->Update(dt);
	UI::getInstance()->Update(dt);


	//for (size_t i = 0; i < CampNPC.size(); i++)
	//{
	//	CampNPC.at(i)->update(dt);
	//}
	if (!UI::getInstance()->isPauseOpen() && !Inventory::getInstance()->isInventoryOpen())
	{
		double c_posx, c_posy;
		glfwGetCursorPos(Application::m_window, &c_posx, &c_posy);
		glfwSetCursorPos(Application::m_window, width / 2, height / 2);

		glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		double dx, dy;
		dx = dt * double(width / 2 - c_posx);
		dy = dt * double(height / 2 - c_posy);
		camera->Update(dt, dx, dy);

		//for (size_t i = 0; i < CampNPC.size(); i++)
		//{
		//	CampNPC.at(i)->update(dt);
		//}
	}

	for (size_t i = 0; i < (sizeof goatMinionPool) / sizeof(*goatMinionPool); ++i)
	{
		if (goatMinionPool[i]->active)
		{
			if (goatMinionPool[i]->getHp() <= 0)
			{
				goatMinionPool[i]->active = false;
				auto it = std::find(Player::enemies_.begin(), Player::enemies_.end(), goatMinionPool[i]);
				std::swap(*it, Player::enemies_.back());
				//goatMinionPool[i]->CollisionMesh_->pos = Vector3(0, -5, 0);
				Player::enemies_.back() = NULL;
				Player::enemies_.pop_back();
				delete goatMinionPool[i]->CollisionMesh_;
			}
			else
				goatMinionPool[i]->update(dt);
		}
	}

	FramesPerSec = 1 / dt;
	
	//MainMenu.Update(dt);
}

void WorldScene::Render()
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
	//renderEnvironment();

	//Ground Mesh
	modelStack.PushMatrix();
	modelStack.Scale(1000, 1000, 1000);
	modelStack.Rotate(90, -1, 0, 0);
	RenderMeshClass::RenderMesh(meshList[GEO_GroundMesh_RedDirt], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();



	//for (size_t i = 0; i < CampNPC.size(); i++)
	//{
	//	CampNPC.at(i)->render(&projectionStack, &viewStack, &modelStack, m_parameters);
	//}

	for (size_t i = 0; i < Env_Obj.size(); i++)
	{
		modelStack.PushMatrix();
		RenderMeshClass::RenderMesh(Env_Obj.at(i)->CollisionMesh_, true, &projectionStack, &viewStack, &modelStack, m_parameters);
		modelStack.PopMatrix();
		
	}

	for (size_t i = 0; i < (sizeof goatMinionPool) / sizeof(*goatMinionPool); ++i)
	{
		if (goatMinionPool[i]->active)
			goatMinionPool[i]->render(&projectionStack, &viewStack, &modelStack, m_parameters);
	}

	Interactions();

	RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Blueprints: ") + std::to_string(Blueprints::GetBlueprintNumber()) + std::string("/3"), Color(1, 0, 0), 2.f, 68, 57, &projectionStack, &viewStack, &modelStack, m_parameters);

	RenderMeshClass::RenderMesh(meshList[GEO_AXES], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	//MinionAI::MinionAI().render(&projectionStack, &viewStack, &modelStack, m_parameters);

	Player::getInstance()->render(&projectionStack, &viewStack, &modelStack, m_parameters);

	RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::to_string(FramesPerSec), Color(1, 0, 0), 1.5f, 45, 38, &projectionStack, &viewStack, &modelStack, m_parameters);

	UI::getInstance()->renderPause(&projectionStack, &viewStack, &modelStack, m_parameters);
	Inventory::getInstance()->Render(&projectionStack, &viewStack, &modelStack, m_parameters);

}


void WorldScene::Interactions(){


	//Teleporter to Camp base -- Main Scene
	if (Player::getInstance()->getPlayerPosition().x >= 92 && Player::getInstance()->getPlayerPosition().x <= 97 && Player::getInstance()->getPlayerPosition().z <= -8 && Player::getInstance()->getPlayerPosition().z >= -15){

		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to teleport to Camp Base.]"), Color(1, 0, 0), 2.f, 28, 30, &projectionStack, &viewStack, &modelStack, m_parameters);
	}

	if (Application::IsKeyPressed(VK_SPACE)){

		if (Player::getInstance()->getPlayerPosition().x >= 92 && Player::getInstance()->getPlayerPosition().x <= 97 && Player::getInstance()->getPlayerPosition().z <= -8 && Player::getInstance()->getPlayerPosition().z >= -15){

			SceneManager::getInstance()->SetNextSceneID(SceneManager::SCENES::CAMPSCENE);
			SceneManager::getInstance()->SetNextScene();
			Player::getInstance()->setPosition(Vector3(-18.0, 0.0, -0.5));
		}

	}


	//Door to Underground Scene
	if (Player::getInstance()->getPlayerPosition().x >= -88 && Player::getInstance()->getPlayerPosition().x <= -86 && Player::getInstance()->getPlayerPosition().z <= -68 && Player::getInstance()->getPlayerPosition().z >= -70)
	{
		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to enter]"), Color(1, 0, 0), 2.f, 28, 30, &projectionStack, &viewStack, &modelStack, m_parameters);
	}

	if (Application::IsKeyPressed(VK_SPACE))
	{
		if (Player::getInstance()->getPlayerPosition().x >= -88 && Player::getInstance()->getPlayerPosition().x <= -86 && Player::getInstance()->getPlayerPosition().z <= -68 && Player::getInstance()->getPlayerPosition().z >= -70)
		{
			SceneManager::getInstance()->SetNextSceneID(SceneManager::SCENES::UNDERGROUNDSCENE);
			SceneManager::getInstance()->SetNextScene();
			Player::getInstance()->setPosition(Vector3(-36.0, 0.0, 30.0));
		}
	}


	//Tablet -- For 2nd Hint, true hint.
	if (Player::getInstance()->getPlayerPosition().x >= -59 && Player::getInstance()->getPlayerPosition().x <= -58 && Player::getInstance()->getPlayerPosition().z <= 39 && Player::getInstance()->getPlayerPosition().z >= 36){

		if (counter_text_tablet == 0){
		
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to scan.]"), Color(1, 0, 0), 2.f, 30, 30, &projectionStack, &viewStack, &modelStack, m_parameters);
		}
		if (Application::IsKeyPressed(VK_SPACE)){

			counter_text_tablet = 1;
		}
		if (counter_text_tablet == 1){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Data extracted*"), Color(1, 0, 0), 2.f, 10, 37, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Discovered: This tablet holds information of a machine that can"), Color(1, 0, 0), 2.f, 10, 35, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("apparently help to rejuvenate the Earth."), Color(1, 0, 0), 2.f, 10, 33, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("There are apparently three parts to the blueprints required to make this device."), Color(1, 0, 0), 2.f, 10, 31, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("One of the locations is recorded in this device. A hidden lab at somewhere"), Color(1, 0, 0), 2.f, 10, 29, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("around coordinates: x;-80, y;-80 *"), Color(1, 0, 0), 2.f, 10, 27, &projectionStack, &viewStack, &modelStack, m_parameters);

		}
	}


	//Broken Robot -- For 1st Hint, true hint.
	if (Player::getInstance()->getPlayerPosition().x >= 75 && Player::getInstance()->getPlayerPosition().x <= 79 && Player::getInstance()->getPlayerPosition().z <= 74 && Player::getInstance()->getPlayerPosition().z >= 67){
	
		if (counter_text_robot == 0){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to scan.]"), Color(1, 0, 0), 2.f, 30, 30, &projectionStack, &viewStack, &modelStack, m_parameters);
		}
		if (Application::IsKeyPressed(VK_SPACE)){

			counter_text_robot = 1;
		}
		if (counter_text_robot == 1){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Data extracted*"), Color(1, 0, 0), 2.f, 10, 37, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Discovered: This robot appears to be a research assistant "), Color(1, 0, 0), 2.f, 10, 35, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("from a time before Earth was abandoned. One registered area of interest."), Color(1, 0, 0), 2.f, 10, 33, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("New coordinates found: x; 54, y;-84*"), Color(1, 0, 0), 2.f, 10, 31, &projectionStack, &viewStack, &modelStack, m_parameters);
		}
	}

	//Oil Barrel -- Fake Hint, A receipt instead.
	if (Player::getInstance()->getPlayerPosition().x >= -110 && Player::getInstance()->getPlayerPosition().x <= -107 && Player::getInstance()->getPlayerPosition().z <= 90 && Player::getInstance()->getPlayerPosition().z >= 87){
		
		if (counter_text_fakeBlueprints == 0){
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to scan.]"), Color(1, 0, 0), 2.f, 30, 30, &projectionStack, &viewStack, &modelStack, m_parameters);
		}
		if (Application::IsKeyPressed(VK_SPACE)){
			
			counter_text_fakeBlueprints = 1;
		}
		if (counter_text_fakeBlueprints == 1){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Object scanned*"), Color(1, 0, 0), 2.f, 15, 35, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*This appears to be a receipt for barrels of oil. False alarm.* "), Color(1, 0, 0), 2.f, 15, 33, &projectionStack, &viewStack, &modelStack, m_parameters);
		}
	}


	//Blueprint 1 -- Near the dead robot
	if (Player::getInstance()->getPlayerPosition().x >= 30 && Player::getInstance()->getPlayerPosition().x <= 34 && Player::getInstance()->getPlayerPosition().z <= 21 && Player::getInstance()->getPlayerPosition().z >= 18){

		if (Blueprint1 == false){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to scan.]"), Color(1, 0, 0), 2.f, 30, 30, &projectionStack, &viewStack, &modelStack, m_parameters);
			if (Application::IsKeyPressed(VK_SPACE))
			{
				Blueprint1 = true;
				Blueprints::AddBlueprintNumber();
			}
		}
		if (Blueprint1 == true){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Object scanned*"), Color(1, 0, 0), 2.f, 10, 37, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Discovered: This appears to be a blueprint for a part of a machine."), Color(1, 0, 0), 2.f, 10, 35, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Committing to memory... Blueprint saved*"), Color(1, 0, 0), 2.f, 10, 33, &projectionStack, &viewStack, &modelStack, m_parameters);
		}
	}

	//Blueprint 2 -- Behind the oil barrels.
	if (Player::getInstance()->getPlayerPosition().x >= 103 && Player::getInstance()->getPlayerPosition().x <= 105 && Player::getInstance()->getPlayerPosition().z <= -95 && Player::getInstance()->getPlayerPosition().z >= -98){
		
		if (Blueprint2 == false){
			
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to scan.]"), Color(1, 0, 0), 2.f, 30, 30, &projectionStack, &viewStack, &modelStack, m_parameters);

			if (Application::IsKeyPressed(VK_SPACE)){

				Blueprint2 = true;
				Blueprints::AddBlueprintNumber();
			}
		}
		if (Blueprint2 == true){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Object scanned*"), Color(1, 0, 0), 2.f, 10, 37, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Discovered: This appears to be a blueprint for a part of a machine."), Color(1, 0, 0), 2.f, 10, 35, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Committing to memory... Blueprint saved*"), Color(1, 0, 0), 2.f, 10, 33, &projectionStack, &viewStack, &modelStack, m_parameters);
		}
	}
}

void WorldScene::Exit()
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

void WorldScene::RenderSkybox()
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

void WorldScene::initEnvironment()
{
	//Outer Row of Buildings 1------------------------------------- START
	EnvironmentObj* OuterBuilding1_BigApartment = new EnvironmentObj(MeshBuilder::GenerateOBJ("Big Apartment", "OBJ//WorldScene//OuterBuildingRow1--BigApartment_OBJ.obj"));
	OuterBuilding1_BigApartment->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//BigApartment.tga");
	EnvironmentObj* OuterBuilding1_Ruins = new EnvironmentObj(MeshBuilder::GenerateOBJ("Ruins", "OBJ//WorldScene//OuterBuildingRow1--Ruins_OBJ.obj"));
	OuterBuilding1_Ruins->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Ruins2_UV_Texture.tga");
	EnvironmentObj* OuterBuilding1_BigSkyScraper = new EnvironmentObj(MeshBuilder::GenerateOBJ("Big Skyscraper", "OBJ//WorldScene//OuterBuildingRow1--BigSkyScraper_OBJ.obj"));
	OuterBuilding1_BigSkyScraper->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Building3UV.tga");
	EnvironmentObj* OuterBuilding1_LShapeBuilding = new EnvironmentObj(MeshBuilder::GenerateOBJ("L Shape Building", "OBJ//WorldScene//OuterBuildingRow1--LShapeApartment_OBJ.obj"));
	OuterBuilding1_LShapeBuilding->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Building4UV.tga");
	EnvironmentObj* OuterBuilding1_PresentHouse = new EnvironmentObj(MeshBuilder::GenerateOBJ("Present House", "OBJ//WorldScene//OuterBuildingRow1--PresentHouse_OBJ.obj"));
	OuterBuilding1_PresentHouse->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//PresentHouse.tga");
	EnvironmentObj* OuterBuilding1_Truck = new EnvironmentObj(MeshBuilder::GenerateOBJ("Truck", "OBJ//WorldScene//OuterBuildingRow1--Truck_OBJ.obj"));
	OuterBuilding1_Truck->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//DestroyedVehicleUV.tga");
	EnvironmentObj* OuterBuilding1_Tree = new EnvironmentObj(MeshBuilder::GenerateOBJ("Tree", "OBJ//WorldScene//OuterBuildingRow1--Tree_OBJ.obj"));
	OuterBuilding1_Tree->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Tree_MediumSize_UV_Texture.tga");

	Env_Obj.push_back(OuterBuilding1_BigApartment);
	Env_Obj.push_back(OuterBuilding1_Ruins);
	Env_Obj.push_back(OuterBuilding1_BigSkyScraper);
	Env_Obj.push_back(OuterBuilding1_LShapeBuilding);
	Env_Obj.push_back(OuterBuilding1_PresentHouse);
	Env_Obj.push_back(OuterBuilding1_Truck);
	Env_Obj.push_back(OuterBuilding1_Tree);
	//Outer Row of Buildings 1------------------------------------- END




	//Outer Row of Buildings 2--------------------------------------START
	EnvironmentObj* OuterBuilding2_BigApartment = new EnvironmentObj(MeshBuilder::GenerateOBJ("Big Apartment", "OBJ//WorldScene//OuterBuildingRow2--BigApartment_OBJ.obj"));
	OuterBuilding2_BigApartment->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//BigApartment.tga");
	EnvironmentObj* OuterBuilding2_Ruins = new EnvironmentObj(MeshBuilder::GenerateOBJ("Ruins", "OBJ//WorldScene//OuterBuildingRow2--Ruins_OBJ.obj"));
	OuterBuilding2_Ruins->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Ruins2_UV_Texture.tga");
	EnvironmentObj* OuterBuilding2_BigSkyScraper = new EnvironmentObj(MeshBuilder::GenerateOBJ("Big Skyscraper", "OBJ//WorldScene//OuterBuildingRow2--BigSkyscraper_OBJ.obj"));
	OuterBuilding2_BigSkyScraper->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Building3UV.tga");
	EnvironmentObj* OuterBuilding2_LShapeBuilding = new EnvironmentObj(MeshBuilder::GenerateOBJ("L Shape Building", "OBJ//WorldScene//OuterBuildingRow2--LShapeBuilding_OBJ.obj"));
	OuterBuilding2_LShapeBuilding->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Building4UV.tga");
	EnvironmentObj* OuterBuilding2_PresentHouse = new EnvironmentObj(MeshBuilder::GenerateOBJ("Present House", "OBJ//WorldScene//OuterBuildingRow2--PresentHouse_OBJ.obj"));
	OuterBuilding2_PresentHouse->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//PresentHouse.tga");

	Env_Obj.push_back(OuterBuilding2_BigApartment);
	Env_Obj.push_back(OuterBuilding2_Ruins);
	Env_Obj.push_back(OuterBuilding2_BigSkyScraper);
	Env_Obj.push_back(OuterBuilding2_LShapeBuilding);
	Env_Obj.push_back(OuterBuilding2_PresentHouse);
	//Outer Row of Buildings 2--------------------------------------END




	//Outer Row of Buildings 3 ----------------------------------- START
	EnvironmentObj* OuterBuilding3_BigApartment = new EnvironmentObj(MeshBuilder::GenerateOBJ("Big Apartment", "OBJ//WorldScene//OuterBuildingRow3--BigApartment_OBJ.obj"));
	OuterBuilding3_BigApartment->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//BigApartment.tga");
	EnvironmentObj* OuterBuilding3_BigApartment2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Big Apartment", "OBJ//WorldScene//OuterBuildingRow3--BigApartment2_OBJ.obj"));
	OuterBuilding3_BigApartment2->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//BigApartment.tga");
	EnvironmentObj* OuterBuilding3_BigApartment3 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Big Apartment", "OBJ//WorldScene//OuterBuildingRow3--BigApartment3_OBJ.obj"));
	OuterBuilding3_BigApartment3->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//BigApartment.tga");
	EnvironmentObj* OuterBuilding3_Ruins = new EnvironmentObj(MeshBuilder::GenerateOBJ("Ruins", "OBJ//WorldScene//OuterBuildingRow3--Ruins_OBJ.obj"));
	OuterBuilding3_Ruins->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Ruins2_UV_Texture.tga");
	EnvironmentObj* OuterBuilding3_Ruins1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Ruins", "OBJ//WorldScene//OuterBuildingRow3--Ruins1_OBJ.obj"));
	OuterBuilding3_Ruins1->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Ruins1_UV_Texture.tga");
	EnvironmentObj* OuterBuilding3_BigSkyScraper = new EnvironmentObj(MeshBuilder::GenerateOBJ("Big Skyscraper", "OBJ//WorldScene//OuterBuildingRow3--BigSkyScraper_OBJ.obj"));
	OuterBuilding3_BigSkyScraper->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Building3UV.tga");
	EnvironmentObj* OuterBuilding3_LShapeBuilding = new EnvironmentObj(MeshBuilder::GenerateOBJ("L Shape Building", "OBJ//WorldScene//OuterBuildingRow3--LShapeBuilding_OBJ.obj"));
	OuterBuilding3_LShapeBuilding->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Building4UV.tga");
	EnvironmentObj* OuterBuilding3_PresentHouse = new EnvironmentObj(MeshBuilder::GenerateOBJ("Present House", "OBJ//WorldScene//OuterBuildingRow3--PresentHouse_OBJ.obj"));
	OuterBuilding3_PresentHouse->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//PresentHouse.tga");
	EnvironmentObj* OuterBuilding3_Truck = new EnvironmentObj(MeshBuilder::GenerateOBJ("Truck", "OBJ//WorldScene//OuterBuildingRow3--Truck_OBJ.obj"));
	OuterBuilding3_Truck->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//DestroyedVehicleUV.tga");
	EnvironmentObj* OuterBuilding3_Tree = new EnvironmentObj(MeshBuilder::GenerateOBJ("Tree", "OBJ//WorldScene//OuterBuildingRow3--Tree_OBJ.obj"));
	OuterBuilding3_Tree->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Tree_MediumSize_UV_Texture.tga");
	EnvironmentObj* OuterBuilding3_Car = new EnvironmentObj(MeshBuilder::GenerateOBJ("Car", "OBJ//WorldScene//OuterBuildingRow3--Car_OBJ.obj"));
	OuterBuilding3_Car->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Car_Texture.tga");

	Env_Obj.push_back(OuterBuilding3_BigApartment);
	Env_Obj.push_back(OuterBuilding3_BigApartment2);
	Env_Obj.push_back(OuterBuilding3_BigApartment3);
	Env_Obj.push_back(OuterBuilding3_Ruins);
	Env_Obj.push_back(OuterBuilding3_Ruins1);
	Env_Obj.push_back(OuterBuilding3_BigSkyScraper);
	Env_Obj.push_back(OuterBuilding3_LShapeBuilding);
	Env_Obj.push_back(OuterBuilding3_PresentHouse);
	Env_Obj.push_back(OuterBuilding3_Truck);
	Env_Obj.push_back(OuterBuilding3_Tree);
	Env_Obj.push_back(OuterBuilding3_Car);
	//Outer Row of Buildings 3 ----------------------------------- END




	//Outer Row of Buildings 4 ------------------------------------ START
	EnvironmentObj* OuterBuilding4_BigApartment = new EnvironmentObj(MeshBuilder::GenerateOBJ("Big Apartment", "OBJ//WorldScene//OuterBuildingRow4--BigApartment_OBJ.obj"));
	OuterBuilding4_BigApartment->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//BigApartment.tga");
	EnvironmentObj* OuterBuilding4_Ruins = new EnvironmentObj(MeshBuilder::GenerateOBJ("Ruins", "OBJ//WorldScene//OuterBuildingRow4--Ruins_OBJ.obj"));
	OuterBuilding4_Ruins->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Ruins2_UV_Texture.tga");
	EnvironmentObj* OuterBuilding4_BigSkyScraper = new EnvironmentObj(MeshBuilder::GenerateOBJ("Big Skyscraper", "OBJ//WorldScene//OuterBuildingRow4--BigSkyScraper_OBJ.obj"));
	OuterBuilding4_BigSkyScraper->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Building3UV.tga");
	EnvironmentObj* OuterBuilding4_LShapeBuilding = new EnvironmentObj(MeshBuilder::GenerateOBJ("L Shape Building", "OBJ//WorldScene//OuterBuildingRow4--LShapeBuilding_OBJ.obj"));
	OuterBuilding4_LShapeBuilding->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Building4UV.tga");
	EnvironmentObj* OuterBuilding4_Tree = new EnvironmentObj(MeshBuilder::GenerateOBJ("Tree", "OBJ//WorldScene//OuterBuildingRow4--Tree_OBJ.obj"));
	OuterBuilding4_Tree->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Tree_MediumSize_UV_Texture.tga");
	EnvironmentObj* OuterBuilding4_Tree2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Tree", "OBJ//WorldScene//OuterBuildingRow4--Tree2_OBJ.obj"));
	OuterBuilding4_Tree2->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Tree_MediumSize_UV_Texture.tga");

	Env_Obj.push_back(OuterBuilding4_BigApartment);
	Env_Obj.push_back(OuterBuilding4_Ruins);
	Env_Obj.push_back(OuterBuilding4_BigSkyScraper);
	Env_Obj.push_back(OuterBuilding4_LShapeBuilding);
	Env_Obj.push_back(OuterBuilding4_Tree);
	Env_Obj.push_back(OuterBuilding4_Tree2);
	//Outer Row of Buildings 4 ------------------------------------ END




	//Inner Row of Buildings -------------------------------------- START
	EnvironmentObj* TopRight_BigApartment = new EnvironmentObj(MeshBuilder::GenerateOBJ("Top Right", "OBJ//WorldScene//InnerBuilding_TopRight_BigApartment_OBJ.obj"));
	TopRight_BigApartment->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//BigApartment.tga");
	EnvironmentObj* TopMiddle_BigApartment = new EnvironmentObj(MeshBuilder::GenerateOBJ("Top Middle", "OBJ//WorldScene//InnerBuilding_TopMiddle_BigApartment_OBJ.obj"));
	TopMiddle_BigApartment->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//BigApartment.tga");
	EnvironmentObj* TopLeft_BigApartment = new EnvironmentObj(MeshBuilder::GenerateOBJ("Top Left", "OBJ//WorldScene//InnerBuilding_TopLeft_BigApartment_OBJ.obj"));
	TopLeft_BigApartment->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//BigApartment.tga");

	EnvironmentObj* MiddleRight_BigSkyScraper = new EnvironmentObj(MeshBuilder::GenerateOBJ("Middle Right", "OBJ//WorldScene//InnerBuilding_MiddleRight_BigSkyScraper_OBJ.obj"));
	MiddleRight_BigSkyScraper->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Building3UV.tga");
	EnvironmentObj* MiddleRight_BigApartment = new EnvironmentObj(MeshBuilder::GenerateOBJ("Middle Right -- BA", "OBJ//WorldScene//InnerBuilding_MiddleRight_BigApartment_OBJ.obj"));
	MiddleRight_BigApartment->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//BigApartment.tga");
	EnvironmentObj* MiddleMiddle_BigApartment = new EnvironmentObj(MeshBuilder::GenerateOBJ("Middle Middle -- BA", "OBJ//WorldScene//InnerBuilding_MiddleMiddle_BigApartment_OBJ.obj"));
	MiddleMiddle_BigApartment->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//BigApartment.tga");
	EnvironmentObj* MiddleMiddle_BigSkyscraper = new EnvironmentObj(MeshBuilder::GenerateOBJ("Middle Middle", "OBJ//WorldScene//InnerBuilding_MiddleMiddle_BigSkyscraper_OBJ.obj"));
	MiddleMiddle_BigSkyscraper->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Building3UV.tga");

	EnvironmentObj* BottomLeft_BigSkyscraper = new EnvironmentObj(MeshBuilder::GenerateOBJ("Bottom Left", "OBJ//WorldScene//InnerBuilding_BottomLeft_BigSkyscraper_OBJ.obj"));
	BottomLeft_BigSkyscraper->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Building3UV.tga");
	EnvironmentObj* BottomLeft_BigApartment = new EnvironmentObj(MeshBuilder::GenerateOBJ("Bottom Left -- BA", "OBJ//WorldScene//InnerBuilding_BottomLeft_BigApartment_OBJ.obj"));
	BottomLeft_BigApartment->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//BigApartment.tga");


	Env_Obj.push_back(TopRight_BigApartment);
	Env_Obj.push_back(TopMiddle_BigApartment);
	Env_Obj.push_back(TopLeft_BigApartment);

	Env_Obj.push_back(MiddleRight_BigSkyScraper);
	Env_Obj.push_back(MiddleRight_BigApartment);
	Env_Obj.push_back(MiddleMiddle_BigApartment);
	Env_Obj.push_back(MiddleMiddle_BigSkyscraper);

	Env_Obj.push_back(BottomLeft_BigSkyscraper);
	Env_Obj.push_back(BottomLeft_BigApartment);
	//Inner Row of Buildings -------------------------------------- END

	//Door to Underground Lab --------------------------------- START

	EnvironmentObj* UndergroundDoor = new EnvironmentObj(MeshBuilder::GenerateOBJ("Underground Door", "OBJ//UndergroundDoor.obj"));
	UndergroundDoor->CollisionMesh_->textureID = LoadTGA("Image//UndergroundDoor.tga");


	Env_Obj.push_back(UndergroundDoor);

	//Underground_Door = new Teleporter(MeshBuilder::GenerateOBJ("Underground Door", "OBJ//UndergroundDoor.obj"), 5);
	//Underground_Door->CollisionMesh_->textureID = LoadTGA("Image//UndergroundDoor.tga");

	//meshList[GEO_UNDERGROUND_DOOR] = MeshBuilder::GenerateOBJ("Underground Door", "OBJ//UndergroundDoor.obj");
	//meshList[GEO_UNDERGROUND_DOOR]->textureID = LoadTGA("Image//UndergroundDoor.tga");

	//Player::getInstance()->Teleport.push_back(Underground_Door);
	//Door to Underground Lab --------------------------------- END

	//Vehicles ----------------------------------------------- START
	EnvironmentObj* DestroyedTruck = new EnvironmentObj(MeshBuilder::GenerateOBJ("Truck", "OBJ//WorldScene//Vehicle1_OBJ.obj"));
	DestroyedTruck->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//DestroyedVehicleUV.tga");

	Env_Obj.push_back(DestroyedTruck);
	//Vehicles ----------------------------------------------- END

	//Barricades --------------------------------------------- START
	EnvironmentObj* Barricade1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Barricade1", "OBJ//WorldScene//Barricade1_OBJ.obj"));
	Barricade1->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Barricade_UV_Texture.tga");
	EnvironmentObj* Barricade2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Barricade2", "OBJ//WorldScene//Barricade2_OBJ.obj"));
	Barricade2->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Barricade_UV_Texture.tga");
	EnvironmentObj* Barricade3 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Barricade3", "OBJ//WorldScene//Barricade3_OBJ.obj"));
	Barricade3->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//Barricade_UV_Texture.tga");

	Env_Obj.push_back(Barricade1);
	Env_Obj.push_back(Barricade2);
	Env_Obj.push_back(Barricade3);
	//Barricades --------------------------------------------- END




	//Teleporter -------------------------------------------- START
	//WS_Teleporter = new Teleporter(MeshBuilder::GenerateOBJ("Teleporter", "OBJ//WorldScene//Teleporter_OBJ.obj"),0);
	//WS_Teleporter->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//InteractableItem_Teleporter_UV_Texture.tga");

	//meshList[GEO_Teleporter] = MeshBuilder::GenerateOBJ("Teleporter", "OBJ//WorldScene//Teleporter_OBJ.obj");
	//meshList[GEO_Teleporter]->textureID = LoadTGA("Image//WorldScene//InteractableItem_Teleporter_UV_Texture.tga");

	EnvironmentObj* Teleporter = new EnvironmentObj(MeshBuilder::GenerateOBJ("Teleporter", "OBJ//WorldScene//Teleporter_OBJ.obj"));
	Teleporter->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//InteractableItem_Teleporter_UV_Texture.tga");


	Env_Obj.push_back(Teleporter);

	//Player::getInstance()->Teleport.push_back(WS_Teleporter);
	//Teleporter -------------------------------------------- END


}

void WorldScene::initItems()
{
	//Interactable Items ----------------------------------------------- START
	EnvironmentObj* Tablet = new EnvironmentObj(MeshBuilder::GenerateOBJ("Tablet", "OBJ//WorldScene//Tablet_OBJ.obj"));
	Tablet->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//InteractableItem_OffTablet_UV_Texture.tga");
	EnvironmentObj* Robot = new EnvironmentObj(MeshBuilder::GenerateOBJ("Robot", "OBJ//WorldScene//BrokenRobot_OBJ.obj"));
	Robot->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//InteractableItem_BrokenRobot_UV_Texture.tga");
	EnvironmentObj* Blueprint1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Blueprint", "OBJ//Blueprint1.obj"));
	Blueprint1->CollisionMesh_->textureID = LoadTGA("Image//BlueprintUV.tga");
	EnvironmentObj* Blueprint2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Blueprint", "OBJ//Blueprint2.obj"));
	Blueprint2->CollisionMesh_->textureID = LoadTGA("Image//BlueprintUV.tga");
	EnvironmentObj* Blueprint_Fakehint = new EnvironmentObj(MeshBuilder::GenerateOBJ("Blueprint", "OBJ//Blueprint_FakeHint.obj"));
	Blueprint_Fakehint->CollisionMesh_->textureID = LoadTGA("Image//BlueprintUV.tga");
	EnvironmentObj* BrokenGuard = new EnvironmentObj(MeshBuilder::GenerateOBJ("Broken Guard", "OBJ//WorldScene//DestroyedGuard.obj"));


	Env_Obj.push_back(Tablet);
	Env_Obj.push_back(Robot);
	Env_Obj.push_back(Blueprint1);
	Env_Obj.push_back(Blueprint2);
	Env_Obj.push_back(Blueprint_Fakehint);
	Env_Obj.push_back(BrokenGuard);
	//Interactable Items ----------------------------------------------- END
}
void WorldScene::initSkybox()
{
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
}
void WorldScene::initblueprints()
{
	//Oil Barrels (Infront of blueprints) -------------------------------------- START
	EnvironmentObj* OilBarrel1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Oil Barrel1", "OBJ//WorldScene//OilBarrel_infront_blueprints1.obj"));
	OilBarrel1->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//OilBarrerlsUV.tga");
	EnvironmentObj* OilBarrel2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Oil Barrel2", "OBJ//WorldScene//OilBarrel_infront_blueprints2.obj"));
	OilBarrel2->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//OilBarrerlsUV.tga");

	Env_Obj.push_back(OilBarrel1);
	Env_Obj.push_back(OilBarrel2);
	//Oil Barrels (Infront of blueprints) --------------------------------------- END




	//Oil Barrels (Fake hints) ------------------------------------------------ START
	EnvironmentObj* OilBarrel_FakeHint1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Oil Barrel1", "OBJ//WorldScene//OilBarrel_FakeHint1.obj"));
	OilBarrel_FakeHint1->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//OilBarrerlsUV.tga");
	EnvironmentObj* OilBarrel_FakeHint2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Oil Barrel2", "OBJ//WorldScene//OilBarrel_FakeHint2.obj"));
	OilBarrel_FakeHint2->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//OilBarrerlsUV.tga");

	Env_Obj.push_back(OilBarrel_FakeHint1);
	Env_Obj.push_back(OilBarrel_FakeHint2);
	//Oil Barrels (Fake hints) ------------------------------------------------ END


}

void WorldScene::initEnemies()
{
	for (size_t i = 0; i < (sizeof goatMinionPool) / sizeof(*goatMinionPool); ++i)
	{
		MinionAI* tempEnemy = new MinionAI();

		tempEnemy->CollisionMesh_->pos = Vector3(i + rand(), 0, i + rand());
		goatMinionPool[i] = tempEnemy;
		goatMinionPool[i]->active = true;

		Player::getInstance()->addCollisionObject(tempEnemy);
		Player::getInstance()->enemies_.push_back(tempEnemy);
	}
}

//MinionAI* WorldScene::getInactiveGoatMinion()
//{
//	for (size_t i = 0; i < (sizeof goatMinionPool) / sizeof(*goatMinionPool); ++i)
//	{
//		if (!goatMinionPool[i]->active)
//			return goatMinionPool[i];
//	}
//	return NULL;
//}