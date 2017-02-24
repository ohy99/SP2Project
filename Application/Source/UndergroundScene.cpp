#include "UndergroundScene.h"
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


//UndergroundScene::Text_Data UndergroundScene::Text[TEXT_TYPE::Text_Count];
//unsigned UndergroundScene::m_parameters[U_TOTAL];
MS UndergroundScene::modelStack, UndergroundScene::viewStack, UndergroundScene::projectionStack;

//std::vector<GameObject*> UndergroundScene::Game_Objects_(10, NULL);

//std::vector<GameObject*> UndergroundScene::Game_Objects_(10, NULL);
//UI renderMeshOnScreen;
std::vector<EnvironmentObj*> UndergroundScene::Env_Obj;
Teleporter* UndergroundScene::Stairs;
//std::vector<NPC*> UndergroundScene::CampNPC;




UndergroundScene::UndergroundScene()
{
}

UndergroundScene::~UndergroundScene()
{
}

void UndergroundScene::Init()
{
	// Init VBO here

	glClearColor(0.0f, 0.5f, 0.66f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Load vertex and fragment shaders
	//m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Blending.fragmentshader");
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");



	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");

	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");

	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	glUseProgram(m_programID);

	glUniform1i(m_parameters[U_NUMLIGHTS], 4);



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




	//INIT TEXTES
	Text[TEXT_TYPE::Calibri].Text_Mesh = MeshBuilder::GenerateText("Calibri", 16, 16);
	Text[TEXT_TYPE::Calibri].Text_Mesh->textureID = LoadTGA("Image//calibri.tga");
	LoadTextData("Image//Calibri Data.csv", Text[TEXT_TYPE::Calibri].TextWidth);
	Text[TEXT_TYPE::Chiller].Text_Mesh = MeshBuilder::GenerateText("Chiller", 16, 16);
	Text[TEXT_TYPE::Chiller].Text_Mesh->textureID = LoadTGA("Image//Chiller.tga");
	LoadTextData("Image//Chiller Data.csv", Text[TEXT_TYPE::Chiller].TextWidth);
	Text[TEXT_TYPE::Century].Text_Mesh = MeshBuilder::GenerateText("Century", 16, 16);
	Text[TEXT_TYPE::Century].Text_Mesh->textureID = LoadTGA("Image//Century.tga");
	LoadTextData("Image//Century Data.csv", Text[TEXT_TYPE::Century].TextWidth);
	Text[TEXT_TYPE::ScriptMTBold].Text_Mesh = MeshBuilder::GenerateText("ScriptMTBold", 16, 16);
	Text[TEXT_TYPE::ScriptMTBold].Text_Mesh->textureID = LoadTGA("Image//Script MT Bold.tga");
	LoadTextData("Image//Script MT Bold Data.csv", Text[TEXT_TYPE::ScriptMTBold].TextWidth);
	Text[TEXT_TYPE::SegoeMarker].Text_Mesh = MeshBuilder::GenerateText("SegoeMarker", 16, 16);
	Text[TEXT_TYPE::SegoeMarker].Text_Mesh->textureID = LoadTGA("Image//Segoe Marker.tga");
	LoadTextData("Image//Segoe Marker Data.csv", Text[TEXT_TYPE::SegoeMarker].TextWidth);

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


	//Ground Mesh ---- Mossy Stone --------------- Start
	meshList[GEO_MOSSY_GROUND] = MeshBuilder::GenerateGround("Ground Mesh", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_MOSSY_GROUND]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_MOSSY_GROUND]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MOSSY_GROUND]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_MOSSY_GROUND]->material.kShininess = 1.0f;
	meshList[GEO_MOSSY_GROUND]->textureID = LoadTGA("Image//Mossy_Ground.tga");
	//Ground Mesh ---- Mossy Stone ----------------- End




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

	//Walls ----------------------------- Start
	EnvironmentObj* Walls1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls1.obj"));
	Walls1->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls2.obj"));
	Walls2->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls3 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls3.obj"));
	Walls3->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls4 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls4.obj"));
	Walls4->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls5 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls5.obj"));
	Walls5->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls6 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls6.obj"));
	Walls6->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls7 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls7.obj"));
	Walls7->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls8 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls8.obj"));
	Walls8->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls9 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls9.obj"));
	Walls9->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls10 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls10.obj"));
	Walls10->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls11 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls11.obj"));
	Walls11->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls12 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls12.obj"));
	Walls12->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls13 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls13.obj"));
	Walls13->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");
	EnvironmentObj* Walls14 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wall", "OBJ//Walls14.obj"));
	Walls14->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall_Door.tga");

	Env_Obj.push_back(Walls1);
	Env_Obj.push_back(Walls2);
	Env_Obj.push_back(Walls3);
	Env_Obj.push_back(Walls4);
	Env_Obj.push_back(Walls5);
	Env_Obj.push_back(Walls6);
	Env_Obj.push_back(Walls7);
	Env_Obj.push_back(Walls8);
	Env_Obj.push_back(Walls9);
	Env_Obj.push_back(Walls10);
	Env_Obj.push_back(Walls11);
	Env_Obj.push_back(Walls12);
	Env_Obj.push_back(Walls13);
	Env_Obj.push_back(Walls14);

	//Walls ----------------------------- End

	//Ceiling --- Start

	EnvironmentObj* Ceiling = new EnvironmentObj(MeshBuilder::GenerateOBJ("Ceiling", "OBJ//Ceiling.obj"));
	Ceiling->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Ceiling.tga");

	Env_Obj.push_back(Ceiling);

	//Ceiling --- End

	//Stairs --- Start
	EnvironmentObj* Stairs = new EnvironmentObj(MeshBuilder::GenerateOBJ("Stairs", "OBJ//Stairs.obj"));
	Stairs->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Wall.tga");

	Env_Obj.push_back(Stairs);
	//Stairs = new Teleporter(MeshBuilder::GenerateOBJ("Stairs", "OBJ//Stairs.obj"), 1);
	//Stairs->CollisionMesh_->textureID = LoadTGA("Image//Mossy_Ground.tga");

	//meshList[GEO_STAIRS] = MeshBuilder::GenerateOBJ("Stairs", "OBJ//Stairs.obj");
	//meshList[GEO_STAIRS]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	//meshList[GEO_STAIRS]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	//meshList[GEO_STAIRS]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	//meshList[GEO_STAIRS]->material.kShininess = 1.0f;
	//meshList[GEO_STAIRS]->textureID = LoadTGA("Image//Mossy_Ground.tga");

	//Player::getInstance()->Teleport.push_back(Stairs);
	//Stairs --- End

	//Furniture ------ Start
	EnvironmentObj* Bed1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Bed", "OBJ//Bed1.obj"));
	Bed1->CollisionMesh_->textureID = LoadTGA("Image//BedUV.tga");
	EnvironmentObj* Bed2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Bed", "OBJ//Bed2.obj"));
	Bed2->CollisionMesh_->textureID = LoadTGA("Image//BedUV.tga");
	EnvironmentObj* Bed3 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Bed", "OBJ//Bed3.obj"));
	Bed3->CollisionMesh_->textureID = LoadTGA("Image//BedUV.tga");
	EnvironmentObj* Desk1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Desk", "OBJ//ComDesk1.obj"));
	Desk1->CollisionMesh_->textureID = LoadTGA("Image//ComDeskUV.tga");
	EnvironmentObj* Desk2 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Desk", "OBJ//ComDesk2.obj"));
	Desk2->CollisionMesh_->textureID = LoadTGA("Image//ComDeskUV.tga");
	EnvironmentObj* Desk3 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Desk", "OBJ//ComDesk3.obj"));
	Desk3->CollisionMesh_->textureID = LoadTGA("Image//ComDeskUV.tga");
	EnvironmentObj* Wardrobe = new EnvironmentObj(MeshBuilder::GenerateOBJ("Wardrobe", "OBJ//Wardrobe.obj"));
	Wardrobe->CollisionMesh_->textureID = LoadTGA("Image//WardrobeUV.tga");

	Env_Obj.push_back(Bed1);
	Env_Obj.push_back(Bed2);
	Env_Obj.push_back(Bed3);
	Env_Obj.push_back(Desk1);
	Env_Obj.push_back(Desk2);
	Env_Obj.push_back(Desk3);
	Env_Obj.push_back(Wardrobe);
	//Furniture ------ End

	//Blueprint 3 ----- Start
	EnvironmentObj* Blueprint = new EnvironmentObj(MeshBuilder::GenerateOBJ("Blueprint", "OBJ//Blueprint3.obj"));
	Blueprint->CollisionMesh_->textureID = LoadTGA("Image//BlueprintUV.tga");

	Env_Obj.push_back(Blueprint);
	//Blueprint 3 ----- End

	for (auto it : Env_Obj)
		Player::addCollisionObject(it);



	UI::getInstance()->Init();


	camera = new Camera3;
	camera->Init(Vector3(0, 0, 7), Vector3(0, 0, 0), Vector3(0, 1, 0));


	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void UndergroundScene::Update(double dt)
{


	int width, height;
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

	//if (Application::IsKeyPressed('1'))
	//{
	//	SceneManager::getInstance()->SetNextSceneID(0);
	//	SceneManager::getInstance()->SetNextScene();
	//}

	bool fpsonce = false;
	if (Application::IsKeyPressed('V') && fpsonce == false)
	{
		camera = FPSCam::getInstance();
		fpsonce = true;
	}

	UI::getInstance()->Update(dt);


	if (!UI::getInstance()->isPauseOpen() && !Inventory::getInstance()->isInventoryOpen())
	{

		double c_posx, c_posy;
		glfwGetCursorPos(Application::m_window, &c_posx, &c_posy);
		glfwSetCursorPos(Application::m_window, width / 2, height / 2);

		glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		Player::getInstance()->update(dt, camera);

		double dx, dy;
		dx = dt * double(width / 2 - c_posx);
		dy = dt * double(height / 2 - c_posy);
		camera->Update(dt, dx, dy);

		//for (size_t i = 0; i < CampNPC.size(); i++)
		//{
		//	CampNPC.at(i)->update(dt);
		//}
	}

	FramesPerSec = 1 / dt;
}

void UndergroundScene::Render()
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

	RenderSkybox();
	//	renderEnvironment();

	//Ground Mesh
	modelStack.PushMatrix();
	modelStack.Scale(1000, 1000, 1000);
	modelStack.Rotate(90, -1, 0, 0);
	RenderMeshClass::RenderMesh(meshList[GEO_MOSSY_GROUND], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	for (size_t i = 0; i < Env_Obj.size(); i++)
	{
		modelStack.PushMatrix();
		RenderMeshClass::RenderMesh(Env_Obj.at(i)->CollisionMesh_, true, &projectionStack, &viewStack, &modelStack, m_parameters);
		modelStack.PopMatrix();
<<<<<<< HEAD

	}

	Interactions();
=======
	}
>>>>>>> a61f1ba9b91ec54e86018ebc4ab8c171a8b2fb4d

	Interactions();
	//for (size_t i = 0; i < CampNPC.size(); i++)
	//{
	//	CampNPC.at(i)->render(&projectionStack, &viewStack, &modelStack, m_parameters);
	//}

	RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Blueprints: ") + std::to_string(Blueprints::GetBlueprintNumber()) + std::string("/3"), Color(1, 0, 0), 2.f, 68, 57, &projectionStack, &viewStack, &modelStack, m_parameters);

	UI::getInstance()->renderPause(&projectionStack, &viewStack, &modelStack, m_parameters);
	Player::getInstance()->render(&projectionStack, &viewStack, &modelStack, m_parameters);
	RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::to_string(FramesPerSec), Color(1, 0, 0), 1.5f, 45, 38, &projectionStack, &viewStack, &modelStack, m_parameters);
}


void UndergroundScene::Interactions()
{
	//Return to World Scene
	if (Player::getInstance()->getPlayerPosition().x >= -39 && Player::getInstance()->getPlayerPosition().x <= -33 && Player::getInstance()->getPlayerPosition().z <= 33 && Player::getInstance()->getPlayerPosition().z >= 26)
	{
		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to exit.]"), Color(1, 0, 0), 2.f, 30, 30, &projectionStack, &viewStack, &modelStack, m_parameters);
	}

	if (Application::IsKeyPressed(VK_SPACE))
	{ 
		if (Player::getInstance()->getPlayerPosition().x >= -39 && Player::getInstance()->getPlayerPosition().x <= -33 && Player::getInstance()->getPlayerPosition().z <= 33 && Player::getInstance()->getPlayerPosition().z >= 26)
		{
			SceneManager::getInstance()->SetNextSceneID(2);
			SceneManager::getInstance()->SetNextScene();

			Player::getInstance()->setPosition(Vector3(-87.0, 0.0, -69.0));
		}
	}

	//Blueprint 3
	if (Player::getInstance()->getPlayerPosition().x >= 41.5 && Player::getInstance()->getPlayerPosition().x <= 43 && Player::getInstance()->getPlayerPosition().z <= 39 && Player::getInstance()->getPlayerPosition().z >= 37){

		if (Blueprint3 == false){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to scan.]"), Color(1, 0, 0), 2.f, 30, 30, &projectionStack, &viewStack, &modelStack, m_parameters);
			if (Application::IsKeyPressed(VK_SPACE))
			{
				Blueprint3 = true;
				Blueprints::AddBlueprintNumber();
			}
		}
		if (Blueprint3 == true){

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Object scanned*"), Color(1, 0, 0), 2.f, 10, 37, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Discovered: This appears to be a blueprint for a part of a machine."), Color(1, 0, 0), 2.f, 10, 35, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("Committing to memory... Blueprint saved*"), Color(1, 0, 0), 2.f, 10, 33, &projectionStack, &viewStack, &modelStack, m_parameters);
		}
	}
}

void UndergroundScene::Exit()
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
void UndergroundScene::RenderSkybox()
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
