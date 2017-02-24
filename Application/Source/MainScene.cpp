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

#include "RenderMesh.h"

#include "UI.h"
#include "Blueprints.h"

//MainScene::Text_Data MainScene::Text[TEXT_TYPE::Text_Count];
//unsigned MainScene::m_parameters[U_TOTAL];
MS MainScene::modelStack, MainScene::viewStack, MainScene::projectionStack;

//std::vector<GameObject*> MainScene::Game_Objects_(10, NULL);

//std::vector<GameObject*> MainScene::Game_Objects_(10, NULL);
UI renderMeshOnScreen;
std::vector<EnvironmentObj*> MainScene::Env_Obj;
std::vector<NPC*> MainScene::CampNPC;
Teleporter* MainScene::MS_Teleporter;
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


	//Ground Mesh ---- Red Dirt --------------- Start
	meshList[GEO_GroundMesh_RedDirt] = MeshBuilder::GenerateGround("Ground Mesh", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_GroundMesh_RedDirt]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GroundMesh_RedDirt]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_GroundMesh_RedDirt]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_GroundMesh_RedDirt]->material.kShininess = 1.0f;
	meshList[GEO_GroundMesh_RedDirt]->textureID = LoadTGA("Image//GroundMesh_RedDirt_Texture.tga");
	//Ground Mesh ---- Red Dirt ----------------- End




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
	//MS_Teleporter = new Teleporter(MeshBuilder::GenerateOBJ("Teleporter", "OBJ//Teleporter_OBJ.obj"), 1);
	//MS_Teleporter->CollisionMesh_->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");

	EnvironmentObj* Teleporter = new EnvironmentObj(MeshBuilder::GenerateOBJ("Teleporter", "OBJ//Teleporter_OBJ.obj"));
	Teleporter->CollisionMesh_->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");

	//Teleporter1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Teleporter1", "OBJ//Teleporter1_OBJ.obj"));
	//Teleporter1->CollisionMesh_->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");

	//meshList[GEO_Teleporter] = MeshBuilder::GenerateOBJ("Teleporter", "OBJ//Teleporter_OBJ.obj");
	//meshList[GEO_Teleporter]->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");
	

	Env_Obj.push_back(Teleporter);
	//Env_Obj.push_back(Teleporter1);

	//Player::getInstance()->Teleport.push_back(MS_Teleporter);
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




	for (auto it : Env_Obj)
		Player::addCollisionObject(it);



	renderUI.Init();
	wasEscPressed = false;
	isPause = false;
	//MainMenu.Init();

	camera = new Camera3;
	camera->Init(Vector3(0, 0, 7), Vector3(0, 0, 0), Vector3(0, 1, 0));


	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void MainScene::Update(double dt)
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

	bool fpsonce = false;
	if (Application::IsKeyPressed('V') && fpsonce == false)
	{
		camera = FPSCam::getInstance();
		fpsonce = true;
	}



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

	for (size_t i = 0; i < CampNPC.size(); i++)
	{
		CampNPC.at(i)->update(dt);
	}


	if (!isPause/* && !MainMenu.isMainMenu*/)
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


	//if (Application::IsKeyPressed('5'))
	//{
	//	SceneManager::getInstance()->SetNextSceneID(5);
	//	SceneManager::getInstance()->SetNextScene();
	//}

	//if (Application::IsKeyPressed('6'))
	//{
	//	SceneManager::getInstance()->SetNextSceneID(1);
	//	SceneManager::getInstance()->SetNextScene();
	//}
	//if (Application::IsKeyPressed('7'))
	//{
	//	SceneManager::getInstance()->SetNextSceneID(4);
	//	SceneManager::getInstance()->SetNextScene();
	//}
	//if (Application::IsKeyPressed('8'))
	//{
	//	SceneManager::getInstance()->SetNextSceneID(3);
	//	SceneManager::getInstance()->SetNextScene();
	//}
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

	//if (MainMenu.isMainMenu)
	//	MainMenu.Render();

	//else
	//{
		RenderMeshClass::RenderMesh(meshList[GEO_AXES], false, &projectionStack, &viewStack, &modelStack, m_parameters);
		Player::getInstance()->render(&projectionStack, &viewStack, &modelStack, m_parameters);

		RenderSkybox();
		//	renderEnvironment();

		//Ground Mesh
		modelStack.PushMatrix();
		modelStack.Scale(1000, 1000, 1000);
		modelStack.Rotate(90, -1, 0, 0);
		RenderMeshClass::RenderMesh(meshList[GEO_GroundMesh_RedDirt], true, &projectionStack, &viewStack, &modelStack, m_parameters);
		modelStack.PopMatrix();


		for (size_t i = 0; i < CampNPC.size(); i++)
		{
			CampNPC.at(i)->render(&projectionStack, &viewStack, &modelStack, m_parameters);
		}
		RenderBaseCamp();

		Interactions();

	/*	RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], std::to_string(numberOfBlueprints), Color(1, 0, 0), 2, 20, 20, &projectionStack, &viewStack, &modelStack, m_parameters);*/

		if (isPause)
			renderUI.renderPause(&projectionStack, &viewStack, &modelStack, m_parameters);


		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::to_string(FramesPerSec), Color(1, 0, 0), 1.5f, 45, 38, &projectionStack, &viewStack, &modelStack, m_parameters);
		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::to_string(Blueprints::GetBlueprintNumber()), Color(1, 0, 0), 1.5f, 20, 20, &projectionStack, &viewStack, &modelStack, m_parameters);
	//}

}

void MainScene::Interactions(){

	if (Player::getInstance()->getPlayerPosition().x >= -8 && Player::getInstance()->getPlayerPosition().x <= -5 && Player::getInstance()->getPlayerPosition().z <= -11 && Player::getInstance()->getPlayerPosition().z >= -17){

		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to enter.]"), Color(1, 0, 0), 2.f, 35, 24, &projectionStack, &viewStack, &modelStack, m_parameters);
	}

	if (Application::IsKeyPressed(VK_SPACE)){
			
		if (Player::getInstance()->getPlayerPosition().x >= -8 && Player::getInstance()->getPlayerPosition().x <= -5 && Player::getInstance()->getPlayerPosition().z <= -11 && Player::getInstance()->getPlayerPosition().z >= -17){

			SceneManager::getInstance()->SetNextSceneID(2);
			SceneManager::getInstance()->SetNextScene();
		}
	}
	
	if (Player::getInstance()->getPlayerPosition().x >= -21 && Player::getInstance()->getPlayerPosition().x <= -17 && Player::getInstance()->getPlayerPosition().z <= 2 && Player::getInstance()->getPlayerPosition().z >= -2){

		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to Teleport into the City.]"), Color(1, 0, 0), 2.f, 28, 24, &projectionStack, &viewStack, &modelStack, m_parameters);
	}

	if (Application::IsKeyPressed(VK_SPACE)){

		if (Player::getInstance()->getPlayerPosition().x >= -21 && Player::getInstance()->getPlayerPosition().x <= -17 && Player::getInstance()->getPlayerPosition().z <= 2 && Player::getInstance()->getPlayerPosition().z >= -2){

			SceneManager::getInstance()->SetNextSceneID(1);
			SceneManager::getInstance()->SetNextScene();
			Player::getInstance()->setPosition(Vector3(94.0, 0.0, -8.0));
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

//int MainScene::GetBlueprintNumber()
//{
//	return numberOfBlueprints;
//}
//
//void MainScene::SetBlueprintNumber(int new_number)
//{
//	numberOfBlueprints = new_number;
//}