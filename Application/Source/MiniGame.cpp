#include "MiniGame.h"
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
#include "Environment.h"
#include <ctime>
//#include "GameObject.h"

#include "NPC_Doc.h"

#include "RenderMesh.h"

#include "UI.h"

std::vector<bool> isActive;
std::vector<Vector3> roadPosition;
std::vector<bool> isObstacleActive;

std::vector<char> count; // rand the number of lorry on one lane, max 2

std::vector<EnvironmentObj*> MiniGame::Obstacles;

MS MiniGame::modelStack, MiniGame::viewStack, MiniGame::projectionStack;


MiniGame::MiniGame()
{
}

MiniGame::~MiniGame()
{
}

void MiniGame::Init()
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

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 1000.f, 1000.f, 1000.f);



	//meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("LightBall", Color(1.f, 1.f, 1.f), 12, 12, 1.f);
	//meshList[GEO_LIGHTBALL1] = MeshBuilder::GenerateCylinder("LightBall1", Color(1.f, 1.f, 1.f), 12, 1.f, 0.f, 1.f);

	//	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere(...);
	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("image", Color(0.9f, 0.9f, 0.9f), 1.0f, 1.0f);
	//meshList[GEO_QUAD]->textureID = LoadTGA("Image//imagey.tga");
	meshList[GEO_QUAD]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_QUAD]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_QUAD]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_QUAD]->material.kShininess = 1.0f;

	//Skybox ------------ Base Camp Start
	//Left Skybox 
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("Left", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//sky1_left.tga");

	//Right Skybox 
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("Right", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//sky1_right.tga");

	//Front Skybox 
	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("Front", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//sky1_front.tga");

	//Back Skybox 
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("Back", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//sky1_back.tga");

	//Top Skybox 
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("Top", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//sky1_up.tga");

	//Bottom Skybox 
	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("Bottom", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//sky1_down.tga");
	//Skybox ------------- Base Camp End

	roadDistance = 0.f;
	obstaclePosX = 0.f;
	obstaclePosZ = 80.f;

	for (int i = 0; i < 20; i++)
	{
		road = MeshBuilder::GenerateOBJ("road", "OBJ//road.obj");
		meshList[ROAD] = road;
		road->textureID = LoadTGA("Image//road.tga");

		counter = rand() % 2; // rand 0 - 1

		roadPosition.push_back(Vector3(0.f, 0.f, roadDistance));
		isActive.push_back(true);
		count.push_back(counter);
		roadDistance += 10.f;
	}

	srand(time(NULL));

	for (int i = 0; i < 16; i++)
	{
		lorry = new EnvironmentObj(MeshBuilder::GenerateOBJ("lorry", "OBJ//lorry.obj"));
		lorry->CollisionMesh_->textureID = LoadTGA("Image//lorry.tga");

		Obstacles.push_back(lorry);
		isObstacleActive.push_back(false);
	}

	for (size_t i = 0; i < Obstacles.size(); i++)
	{
		obstaclePosX = rand() % 3; // rand 0 - 1
		order[0] = obstaclePosX;

		obstaclePosX = rand() % 3; // rand 0 - 1

		if (obstaclePosX == order[0])
		{
			order[1] = (int)(obstaclePosX + 1) % 3;
		}

		if (!isObstacleActive[i])
		{
			isObstacleActive[i] = true;

			if ((int)count[i] == 0)
			{
				Obstacles[i]->CollisionMesh_->pos.x = -7.f + ((int)order[0] * 7.f);
				Obstacles[i]->CollisionMesh_->pos.z = obstaclePosZ;
			}


			else if ((int)count[i] == 1)
			{
				if (i + 1 <= 15)
				{
					Obstacles[i]->CollisionMesh_->pos.x = -7.f + ((int)order[0] * 7.f);
					Obstacles[i]->CollisionMesh_->pos.z = obstaclePosZ;
					Obstacles[i + 1]->CollisionMesh_->pos.x = -7.f + ((int)order[1] * 7.f);
					Obstacles[i + 1]->CollisionMesh_->pos.z = obstaclePosZ;

					isObstacleActive[i + 1] = true;
					i++;
				}

				else
				{
					Obstacles[i]->CollisionMesh_->pos.x = -7.f + ((int)order[1] * 7.f);
					Obstacles[i]->CollisionMesh_->pos.z = obstaclePosZ;

					isObstacleActive[i] = true;
				}
			}
		}

		obstaclePosZ += 20.f;
	}

	for (size_t i = 0; i < Obstacles.size(); i++)
	{
		if (isObstacleActive[i])
			MGPlayer::addCollisionObject(Obstacles[i]);

		else
			continue;
	}

	UI::getInstance()->Init();
	MGPlayer::getInstance()->Init();

	score = 0.f;
	bonusScore = 0.2f; // Default score multiplier

	camera = new CameraMG;
	camera->Init(Vector3(0.f, 0.f, 7.f), Vector3(0.f, 0.f, 50.f), Vector3(0.f, 1.f, 0.f));

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	// Hide the mouse and enable unlimited mouvement

	MGList[SCREEN] = MeshBuilder::GenerateQuad("image", Color(0.9f, 0.9f, 0.9f), 1.0f, 1.0f);
	MGList[SCREEN]->textureID = LoadTGA("Image//inventory.tga");

	MGList[START] = MeshBuilder::GenerateQuad("image", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	MGList[START]->textureID = LoadTGA("Image//start.tga");
	MGList[START]->setHb(true, Vector3(-200.f, -50.f, 0.f), Vector3(200.f, 50.f, 2.f),
		Vector3((float)Application::getWindowWidth() * 0.5f, (float)Application::getWindowHeight() * 0.5f, 0),
		Vector3(0.f, 1.f, 0.f));


	MGList[RESTART] = MeshBuilder::GenerateQuad("image", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	MGList[RESTART]->textureID = LoadTGA("Image//restart.tga");
	MGList[RESTART]->setHb(true, Vector3(-200.f, -50.f, 0.f), Vector3(200.f, 50.f, 2.f),
		Vector3((float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 768) * 400, 0.f),
		Vector3(0.f, 1.f, 0.f));

	MGList[QUIT] = MeshBuilder::GenerateQuad("image", Color(1.f, 1.f, 1.f), 1.0f, 1.0f);
	MGList[QUIT]->textureID = LoadTGA("Image//quit.tga");
	MGList[QUIT]->setHb(true, Vector3(-200.f, -50.f, 0.f), Vector3(200.f, 50.f, 2.f),
		Vector3((float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 768) * (Application::getWindowHeight() - 200), 0.f),
		Vector3(0.f, 1.f, 0.f));
}

void MiniGame::Update(double dt)
{
	glfwGetWindowSize(Application::m_window, &width, &height);

	UI::getInstance()->Update(dt);
	MGPlayer::getInstance()->Update(dt, camera);

	if (!MGPlayer::getInstance()->gameStarted() && !MGPlayer::getInstance()->isDead())
		glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (!UI::getInstance()->isPauseOpen() && MGPlayer::getInstance()->gameStarted() && !MGPlayer::getInstance()->isDead())
	{
		glfwGetCursorPos(Application::m_window, &x, &y);
		glfwSetCursorPos(Application::m_window, width / 2, height / 2);

		//glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		x = dt * double(width / 2 - x);
		y = dt * double(height / 2 - y);
		camera->Update(dt, x, y);
	}

	for (int i = 0; i < 20; i++)
	{
		if (isActive[i])
		{
			if (camera->getPosition().z > roadPosition[i].z + 10.f)
				isActive[i] = false;
		}

		else
		{
			roadPosition[i].z += 200;
			isActive[i] = true;
		}
	}

	for (size_t i = 0; i < Obstacles.size(); i++)
	{
		if (isObstacleActive[i])
		{
			if (camera->getPosition().z > Obstacles[i]->CollisionMesh_->pos.z + 10.f)
				isObstacleActive[i] = false;
		}

		else
		{
			obstaclePosX = rand() % 3; // rand 0 - 1
			order[0] = obstaclePosX;

			obstaclePosX = rand() % 3; // rand 0 - 1

			if (obstaclePosX == order[0])
			{
				order[1] = (int)(obstaclePosX + 1) % 3;
			}

			if (!isObstacleActive[i])
			{
				isObstacleActive[i] = true;

				if ((int)count[i] == 0)
				{
					Obstacles[i]->CollisionMesh_->pos.x = -7.f + ((int)order[0] * 7.f);
					Obstacles[i]->CollisionMesh_->pos.z = obstaclePosZ;
				}


				else if ((int)count[i] == 1)
				{
					if (i + 1 <= 15)
					{
						Obstacles[i]->CollisionMesh_->pos.x = -7.f + ((int)order[0] * 7.f);
						Obstacles[i]->CollisionMesh_->pos.z = obstaclePosZ;
						Obstacles[i + 1]->CollisionMesh_->pos.x = -7.f + ((int)order[1] * 7.f);
						Obstacles[i + 1]->CollisionMesh_->pos.z = obstaclePosZ;

						isObstacleActive[i + 1] = true;
						i++;
					}

					else
					{
						Obstacles[i]->CollisionMesh_->pos.x = -7.f + ((int)order[1] * 7.f);
						Obstacles[i]->CollisionMesh_->pos.z = obstaclePosZ;

						isObstacleActive[i] = true;
					}
				}
			}

			obstaclePosZ += 20.f;
			isObstacleActive[i] = true;
		}
	}

	score = MGPlayer::getInstance()->playerScore(bonusScore);
	GameState();

	FramesPerSec = 1 / dt; 

	if (Application::IsKeyPressed('1'))
	{
		SceneManager::getInstance()->SetNextSceneID(0);
		SceneManager::getInstance()->SetNextScene();
	}
}

void MiniGame::Render()
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

	MGPlayer::getInstance()->Render(&projectionStack, &viewStack, &modelStack, m_parameters);

	modelStack.PushMatrix();
	modelStack.Translate(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	RenderSkybox();
	modelStack.PopMatrix();

	RenderMiniGame();

	UI::getInstance()->renderPause(&projectionStack, &viewStack, &modelStack, m_parameters);

	RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::to_string(FramesPerSec), Color(1.f, 0.f, 0.f), 1.5f, 45.f, 38.f, &projectionStack, &viewStack, &modelStack, m_parameters);

	if (MGPlayer::getInstance()->gameStarted())
	{
		Score = "Total score:" + std::to_string(score);
		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], Score, Color(1.f, 1.f, 1.f), 3.8f, 48.f, 56.5f, &projectionStack, &viewStack, &modelStack, m_parameters);
	}
}

void MiniGame::Exit()
{
	for (size_t i = 0; i < GEOMETRY_TYPE::NUM_GEOMETRY; i++)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	/*delete camera;*/

	//delete road;
	delete lorry->CollisionMesh_;

	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void MiniGame::RenderMiniGame()
{
	// Road
	for (int i = 0; i < 20; i++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(0.f, 0.f, roadPosition[i].z);
		RenderMeshClass::RenderMesh(meshList[ROAD], false, &projectionStack, &viewStack, &modelStack, m_parameters);
		modelStack.PopMatrix();
	}

	// Obstacles
	//modelStack.PushMatrix();
	//modelStack.Translate(0.f, 0.f, 60.f);
	//RenderMeshClass::RenderMesh(meshList[LORRY], false, &projectionStack, &viewStack, &modelStack, m_parameters);
	//modelStack.PopMatrix();

	for (size_t i = 0; i < Obstacles.size(); i++)
	{
		if (isObstacleActive[i])
		{
			modelStack.PushMatrix();
			modelStack.Translate(Obstacles.at(i)->CollisionMesh_->pos.x, Obstacles.at(i)->CollisionMesh_->pos.y, Obstacles.at(i)->CollisionMesh_->pos.z);
			RenderMeshClass::RenderMesh(Obstacles.at(i)->CollisionMesh_, true, &projectionStack, &viewStack, &modelStack, m_parameters);
			modelStack.PopMatrix();
		}
	}

	if (!MGPlayer::getInstance()->gameStarted())
	{
		RenderMeshClass::RenderMeshOnScreen(MGList[SCREEN], (int)Application::getWindowWidth() * 0.5f, (int)Application::getWindowHeight() * 0.5f, 0, (int)(Application::getWindowWidth() / 1024) * 700, (int)(Application::getWindowHeight() / 768) * 700, &projectionStack, &viewStack, &modelStack, m_parameters);
		RenderMeshClass::RenderMeshOnScreen(MGList[START], (int)Application::getWindowWidth() * 0.5f, (int)Application::getWindowHeight() * 0.5f, 0, (int)(Application::getWindowWidth() / 1024) * 400, (int)(Application::getWindowHeight() / 768) * 100, &projectionStack, &viewStack, &modelStack, m_parameters);
	}

	if (MGPlayer::getInstance()->isDead())
	{
		RenderMeshClass::RenderMeshOnScreen(MGList[SCREEN], (int)Application::getWindowWidth() * 0.5f, (int)Application::getWindowHeight() * 0.5f, 0, (int)(Application::getWindowWidth() / 1024) * 700, (int)(Application::getWindowHeight() / 768) * 700, &projectionStack, &viewStack, &modelStack, m_parameters);
		RenderMeshClass::RenderMeshOnScreen(MGList[RESTART], (int)Application::getWindowWidth() * 0.5f, (int)(Application::getWindowHeight() / 768) * 400, 0, (int)(Application::getWindowWidth() / 1024) * 400, (int)(Application::getWindowHeight() / 768) * 100, &projectionStack, &viewStack, &modelStack, m_parameters);
		RenderMeshClass::RenderMeshOnScreen(MGList[QUIT], (int)Application::getWindowWidth() * 0.5f, (int)(Application::getWindowHeight() / 768) * 200.f, 0, (int)(Application::getWindowWidth() / 1024) * 400, (int)(Application::getWindowHeight() / 768) * 100, &projectionStack, &viewStack, &modelStack, m_parameters);
	}
}

void MiniGame::RenderSkybox()
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

void MiniGame::Reset()
{
	MGPlayer::getInstance()->setPlayerPosition(Vector3(0.f, 0.f, 0.f));
	roadDistance = 0.f;
	obstaclePosX = MGPlayer::getInstance()->getPlayerPosition().x;
	obstaclePosZ = MGPlayer::getInstance()->getPlayerPosition().z + 80.f;


	for (int i = 0; i < 20; i++)
	{
		roadPosition[i].z = roadDistance;
		roadDistance += 10.f;
	}

	for (size_t i = 0; i < Obstacles.size(); i++)
	{
		isObstacleActive[i] = false;
	}

	for (size_t i = 0; i < Obstacles.size(); i++)
	{
		obstaclePosX = rand() % 3; // rand 0 - 1
		order[0] = obstaclePosX;

		obstaclePosX = rand() % 3; // rand 0 - 1

		if (obstaclePosX == order[0])
		{
			order[1] = (int)(obstaclePosX + 1) % 3;
		}

		if (!isObstacleActive[i])
		{
			isObstacleActive[i] = true;

			if ((int)count[i] == 0)
			{
				Obstacles[i]->CollisionMesh_->pos.x = -7.f + ((int)order[0] * 7.f);
				Obstacles[i]->CollisionMesh_->pos.z = obstaclePosZ;
			}


			else if ((int)count[i] == 1)
			{
				if (i + 1 <= 15)
				{
					Obstacles[i]->CollisionMesh_->pos.x = -7.f + ((int)order[0] * 7.f);
					Obstacles[i]->CollisionMesh_->pos.z = obstaclePosZ;
					Obstacles[i + 1]->CollisionMesh_->pos.x = -7.f + ((int)order[1] * 7.f);
					Obstacles[i + 1]->CollisionMesh_->pos.z = obstaclePosZ;

					isObstacleActive[i + 1] = true;
					i++;
				}

				else
				{
					Obstacles[i]->CollisionMesh_->pos.x = -7.f + ((int)order[1] * 7.f);
					Obstacles[i]->CollisionMesh_->pos.z = obstaclePosZ;

					isObstacleActive[i] = true;
				}
			}
		}

		obstaclePosZ += 20.f;
	}
}

void MiniGame::GameState()
{
	glfwGetCursorPos(Application::m_window, &x, &y);
	leftButton = glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT);
	isLeftMouseButtonPressed = leftButton;

	if (MGPlayer::getInstance()->isDead())
		glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (isLeftMouseButtonPressed && !wasLeftMouseButtonPressed)
	{
		if (MGList[START]->isPointInsideAABB(Position(x, y, 0)) && !MGPlayer::getInstance()->gameStarted())
		{
			MGPlayer::getInstance()->setGameState(true, false, false);
			glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		if (MGList[RESTART]->isPointInsideAABB(Position(x, y, 0)) && MGPlayer::getInstance()->isDead())
		{
			Reset();
			MGPlayer::getInstance()->setGameState(true, true, false);
			glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		if (MGList[QUIT]->isPointInsideAABB(Position(x, y, 0)) && MGPlayer::getInstance()->isDead())
		{
			Reset();
			MGPlayer::getInstance()->setGameState(false, false, false);
			SceneManager::getInstance()->SetNextSceneID(1);
			SceneManager::getInstance()->SetNextScene();
		}

		wasLeftMouseButtonPressed = isLeftMouseButtonPressed;
	}

	if (!isLeftMouseButtonPressed && wasLeftMouseButtonPressed)
		wasLeftMouseButtonPressed = isLeftMouseButtonPressed;
}