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

#include "RenderMesh.h"

#include "UI.h"

//MainScene::Text_Data MainScene::Text[TEXT_TYPE::Text_Count];
//unsigned MainScene::m_parameters[U_TOTAL];
MS MainScene::modelStack, MainScene::viewStack, MainScene::projectionStack;

//std::vector<GameObject*> MainScene::Game_Objects_(10, NULL);

//std::vector<GameObject*> MainScene::Game_Objects_(10, NULL);


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
	meshList[GEO_Barricade1] = MeshBuilder::GenerateOBJ("Barricade", "OBJ//Barricade1_OBJ.obj");
	meshList[GEO_Barricade1]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_Barricade1]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_Barricade1]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_Barricade1]->material.kShininess = 1.0f;
	meshList[GEO_Barricade1]->textureID = LoadTGA("Image//Barricade_UV_Texture.tga");

	meshList[GEO_Barricade2] = MeshBuilder::GenerateOBJ("Barricade", "OBJ//Barricade2_OBJ.obj");
	meshList[GEO_Barricade2]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_Barricade2]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_Barricade2]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_Barricade2]->material.kShininess = 1.0f;
	meshList[GEO_Barricade2]->textureID = LoadTGA("Image//Barricade_UV_Texture.tga");

	meshList[GEO_Barricade3] = MeshBuilder::GenerateOBJ("Barricade", "OBJ//Barricade3_OBJ.obj");
	meshList[GEO_Barricade3]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_Barricade3]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_Barricade3]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_Barricade3]->material.kShininess = 1.0f;
	meshList[GEO_Barricade3]->textureID = LoadTGA("Image//Barricade_UV_Texture.tga");

	meshList[GEO_Barricade4] = MeshBuilder::GenerateOBJ("Barricade", "OBJ//Barricade4_OBJ.obj");
	meshList[GEO_Barricade4]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_Barricade4]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_Barricade4]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_Barricade4]->material.kShininess = 1.0f;
	meshList[GEO_Barricade4]->textureID = LoadTGA("Image//Barricade_UV_Texture.tga");
	//Barricade -------------------- End




	//Teleporter ------------------ START
	meshList[GEO_Teleporter] = MeshBuilder::GenerateOBJ("Teleporter", "OBJ//Teleporter_OBJ.obj");
	meshList[GEO_Teleporter]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_Teleporter]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_Teleporter]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_Teleporter]->material.kShininess = 1.0f;
	meshList[GEO_Teleporter]->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");

	meshList[GEO_Teleporter1] = MeshBuilder::GenerateOBJ("Teleporter", "OBJ//Teleporter1_OBJ.obj");
	meshList[GEO_Teleporter1]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_Teleporter1]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_Teleporter1]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_Teleporter1]->material.kShininess = 1.0f;
	meshList[GEO_Teleporter1]->textureID = LoadTGA("Image//InteractableItem_Teleporter_UV_Texture.tga");
	//Teleporter -----------------------END


	NPC* Doc = new NPC_DOC();
	//Game_Objects_.push_back(Doc);
	CampNPC.push_back(Doc);




	//Medical Tent ------------------- START
	meshList[GEO_MedicalTent] = MeshBuilder::GenerateOBJ("Medical Tent", "OBJ//MedicalTent_OBJ.obj");
	meshList[GEO_MedicalTent]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_MedicalTent]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MedicalTent]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_MedicalTent]->material.kShininess = 1.0f;
	meshList[GEO_MedicalTent]->textureID = LoadTGA("Image//MedicalCamp_UV_Texture.tga");
	//Medical Tent ------------------- END



	//Barracks -------------------- START
	meshList[GEO_Barracks] = MeshBuilder::GenerateOBJ("Barracks", "OBJ//Barracks_OBJ.obj");
	meshList[GEO_Barracks]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_Barracks]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_Barracks]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_Barracks]->material.kShininess = 1.0f;
	meshList[GEO_Barracks]->textureID = LoadTGA("Image//Barracks_UV_Texture.tga");
	//Barracks ------------------------- END




	//Crates ------------------------------- START
	meshList[GEO_Crates] = MeshBuilder::GenerateOBJ("Barracks", "OBJ//Crates_OBJ.obj");
	meshList[GEO_Crates]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_Crates]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_Crates]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_Crates]->material.kShininess = 1.0f;
	meshList[GEO_Crates]->textureID = LoadTGA("Image//Crate_UV_Texture.tga");

	meshList[GEO_Crates1] = MeshBuilder::GenerateOBJ("Barracks", "OBJ//Crates1_OBJ.obj");
	meshList[GEO_Crates1]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_Crates1]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_Crates1]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_Crates1]->material.kShininess = 1.0f;
	meshList[GEO_Crates1]->textureID = LoadTGA("Image//Crate_UV_Texture.tga");
	//Crates ------------------------------------------- END




	//Solar Panels------------------------------------- START
	meshList[GEO_SolarPanel] = MeshBuilder::GenerateOBJ("Barracks", "OBJ//SolarPanel_OBJ.obj");
	meshList[GEO_SolarPanel]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_SolarPanel]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_SolarPanel]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_SolarPanel]->material.kShininess = 1.0f;
	meshList[GEO_SolarPanel]->textureID = LoadTGA("Image//SolarUV_Texture.tga");

	meshList[GEO_SolarPanel1] = MeshBuilder::GenerateOBJ("Barracks", "OBJ//SolarPanel1_OBJ.obj");
	meshList[GEO_SolarPanel1]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_SolarPanel1]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_SolarPanel1]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_SolarPanel1]->material.kShininess = 1.0f;
	meshList[GEO_SolarPanel1]->textureID = LoadTGA("Image//SolarUV_Texture.tga");
	//Solar Panels ------------------------------------- END




	//Powerbox ----------------------------------- START
	meshList[GEO_PowerBox] = MeshBuilder::GenerateOBJ("Barracks", "OBJ//Powerbox_OBJ.obj");
	meshList[GEO_PowerBox]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_PowerBox]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_PowerBox]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_PowerBox]->material.kShininess = 1.0f;
	meshList[GEO_PowerBox]->textureID = LoadTGA("Image//Powerbox_UV_Texture.tga");
	//Powerbox ----------------------------------- END


	renderUI.Init();
	wasEscPressed = false;
	isPause = false;
	MainMenu.Init();

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

	bool once = false;
	if (Application::IsKeyPressed('1') && once == false)
	{
		SceneManager::getInstance()->SetNextScene();
		//SceneManager::getInstance()->SetNextSceneID(0);
		once = true;
	}





	if (isEscPressed && !wasEscPressed) // When you press ESC
	{
		if (!MainMenu.isMainMenu)
		{
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
		}
	}
		
	if (!isEscPressed && wasEscPressed) // When you release the ESC button
		wasEscPressed = isEscPressed;

	Player::getInstance()->update(dt, camera);

	for (size_t i = 0; i < CampNPC.size(); i++)
	{
		CampNPC.at(i)->update(dt);
	}


	if (!isPause && !MainMenu.isMainMenu)
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
	MainMenu.Update(dt);
	
	glfwGetCursorPos(Application::m_window, &x, &y);
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

	std::string posx = "x cur pos :" + std::to_string(x);
	std::string posy = "y cur pos :" + std::to_string(y);

	if (MainMenu.isMainMenu)
	{		

		MainMenu.Render();
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], posx, Color(1, 1, 1), 5, 25, 50, &projectionStack, &viewStack, &modelStack, m_parameters);
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], posy, Color(1, 1, 1), 5, 25, 45, &projectionStack, &viewStack, &modelStack, m_parameters);
	}
		


	else
	{
		RenderMeshClass::RenderMesh(meshList[GEO_AXES], false, &projectionStack, &viewStack, &modelStack, m_parameters);

		Player::getInstance()->render();

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


		if (isPause)
			renderUI.renderPause(&projectionStack, &viewStack, &modelStack, m_parameters);


		RenderBaseCamp();
		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::to_string(FramesPerSec), Color(1, 0, 0), 1.5f, 45, 38, &projectionStack, &viewStack, &modelStack, m_parameters);
	}
}

void MainScene::RenderBaseCamp(){

	//Barricade START ------------------------------------------------- Around the camp
	modelStack.PushMatrix();
	RenderMeshClass::RenderMesh(meshList[GEO_Barricade1], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMeshClass::RenderMesh(meshList[GEO_Barricade2], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMeshClass::RenderMesh(meshList[GEO_Barricade3], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMeshClass::RenderMesh(meshList[GEO_Barricade4], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();
	//Barricade END ------------------------------------------------------

	//Teleporter 1 -- To world map -- Where player find the blueprints
	modelStack.PushMatrix();
	RenderMeshClass::RenderMesh(meshList[GEO_Teleporter], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();
	


	//Teleporter 2 -- To final boss?? -- Fight the final boss to put an item somewhere?? 
	modelStack.PushMatrix();
	RenderMeshClass::RenderMesh(meshList[GEO_Teleporter1], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();



	//Medical Tent
	modelStack.PushMatrix();
	RenderMeshClass::RenderMesh(meshList[GEO_MedicalTent], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	//Barracks
	modelStack.PushMatrix();
	RenderMeshClass::RenderMesh(meshList[GEO_Barracks], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	//Crates
	modelStack.PushMatrix();
	RenderMeshClass::RenderMesh(meshList[GEO_Crates], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	RenderMeshClass::RenderMesh(meshList[GEO_Crates1], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	//Solar panels
	modelStack.PushMatrix();
	RenderMeshClass::RenderMesh(meshList[GEO_SolarPanel], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	RenderMeshClass::RenderMesh(meshList[GEO_SolarPanel1], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

	//Powerbox
	modelStack.PushMatrix();
	RenderMeshClass::RenderMesh(meshList[GEO_PowerBox], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	modelStack.PopMatrix();

}

void MainScene::Exit()
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


//void MainScene::RenderMesh(Mesh *mesh, bool enableLight)
//{
//	Mtx44 MVP, modelView, modelView_inverse_transpose;
//
//	if (mesh->textureID > 0)
//	{
//		glUniform1i(MainScene::m_parameters[MainScene::UNIFORM_TYPE::U_COLOR_TEXTURE_ENABLED], 1);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
//		glUniform1i(MainScene::m_parameters[MainScene::UNIFORM_TYPE::U_COLOR_TEXTURE], 0);
//	}
//	else
//	{
//		glUniform1i(MainScene::m_parameters[MainScene::UNIFORM_TYPE::U_COLOR_TEXTURE_ENABLED], 0);
//	}
//
//
//	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
//	glUniformMatrix4fv(MainScene::m_parameters[MainScene::UNIFORM_TYPE::U_MVP], 1, GL_FALSE, &MVP.a[0]);
//	modelView = viewStack.Top() * modelStack.Top();
//	glUniformMatrix4fv(MainScene::m_parameters[MainScene::UNIFORM_TYPE::U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
//	if (enableLight)
//	{
//
//		glUniform1i(MainScene::m_parameters[MainScene::UNIFORM_TYPE::U_LIGHTENABLED], 1);
//		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
//		glUniformMatrix4fv(MainScene::m_parameters[MainScene::UNIFORM_TYPE::U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);
//
//		//load material
//		glUniform3fv(MainScene::m_parameters[MainScene::UNIFORM_TYPE::U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
//		glUniform3fv(MainScene::m_parameters[MainScene::UNIFORM_TYPE::U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
//		glUniform3fv(MainScene::m_parameters[MainScene::UNIFORM_TYPE::U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
//		glUniform1f(MainScene::m_parameters[MainScene::UNIFORM_TYPE::U_MATERIAL_SHININESS], mesh->material.kShininess);
//	}
//	else
//	{
//		glUniform1i(MainScene::m_parameters[UNIFORM_TYPE::U_LIGHTENABLED], 0);
//	}
//	mesh->Render();
//	if (mesh->textureID > 0)
//	{
//		glBindTexture(GL_TEXTURE_2D, 0);
//	}
//}

//void MainScene::RenderText(Text_Data* TextData, std::string text, Color color)
//{
//	if (!TextData->Text_Mesh || TextData->Text_Mesh->textureID <= 0) //Proper error check
//		return;
//
//	glDisable(GL_DEPTH_TEST);
//	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
//	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
//	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
//	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, TextData->Text_Mesh->textureID);
//	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
//	float nextCharWidth = 0.0f;
//	for (unsigned i = 0; i < text.length(); ++i)
//	{
//		char Character = text[i];
//		Mtx44 characterSpacing;
//		characterSpacing.SetToTranslation(nextCharWidth, 0, 0); //1.0f is the spacing of each character, you may change this value
//		nextCharWidth += TextData->TextWidth[Character];
//		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
//		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
//
//		TextData->Text_Mesh->Render((unsigned)text[i] * 6, 6);
//	}
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
//	glEnable(GL_DEPTH_TEST);
//}
//void MainScene::RenderTextOnScreen(Text_Data* TextData, std::string text, Color color, float size, float x, float y)
//{
//	if (!TextData->Text_Mesh || TextData->Text_Mesh->textureID <= 0) //Proper error check
//		return;
//
//	glDisable(GL_DEPTH_TEST);
//	Mtx44 ortho;
//	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
//	projectionStack.PushMatrix();
//	projectionStack.LoadMatrix(ortho);
//	viewStack.PushMatrix();
//	viewStack.LoadIdentity(); //No need camera for ortho mode
//	modelStack.PushMatrix();
//	modelStack.LoadIdentity(); //Reset modelStack
//	modelStack.Scale(size, size, size);
//	modelStack.Translate(x, y, 0);
//
//	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
//	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
//	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
//	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, TextData->Text_Mesh->textureID);
//	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
//	float nextCharWidth = 0.0f;
//	for (unsigned i = 0; i < text.length(); ++i)
//	{
//		char Character = text[i];
//		Mtx44 characterSpacing;
//		characterSpacing.SetToTranslation(nextCharWidth, 0, 0); //1.0f is the spacing of each character, you may change this value
//		nextCharWidth += TextData->TextWidth[Character];
//		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
//		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
//
//		TextData->Text_Mesh->Render((unsigned)text[i] * 6, 6);
//	}
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
//	glEnable(GL_DEPTH_TEST);
//	projectionStack.PopMatrix();
//	viewStack.PopMatrix();
//	modelStack.PopMatrix();
//}
//void MainScene::RenderMeshOnScreen(Mesh* mesh, int x, int y, int sizex, int sizey)
//{
//	glDisable(GL_DEPTH_TEST);
//	Mtx44 ortho;
//	ortho.SetToOrtho(0, Application::getWindowWidth(), 0, Application::getWindowHeight(), -10, 10); //size of screen UI
//	projectionStack.PushMatrix();
//	projectionStack.LoadMatrix(ortho);
//	viewStack.PushMatrix();
//	viewStack.LoadIdentity(); //No need camera for ortho mode
//	modelStack.PushMatrix();
//	modelStack.LoadIdentity();
//	//to do: scale and translate accordingly
//
//	modelStack.Translate(x, y, 0);
//	modelStack.Scale(sizex, sizey, 1);
//	RenderMesh(mesh, false); //UI should not have light
//	projectionStack.PopMatrix();
//	viewStack.PopMatrix();
//	modelStack.PopMatrix();
//	glEnable(GL_DEPTH_TEST);
//}
