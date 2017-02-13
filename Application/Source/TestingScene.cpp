#include "TestingScene.h"
#include "GL\glew.h"

#include <GLFW\glfw3.h>

#include "shader.hpp"
#include "Mtx44.h"
#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "LoadTextData.h"



Mesh* Test::GroundMesh = 0;

Test::Test()
{
}

Test::~Test()
{
}

void Test::Init()
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
	GroundMesh = NULL;


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

	GroundMesh = MeshBuilder::GenerateQuad("GroundMesh", Color(1.f, 1.f, 1.f), 500, 500);
	//GroundMesh->textureID = LoadTGA("IMAGE//Grassgreen.tga");
	//GroundMesh->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	//GroundMesh->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	//GroundMesh->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	//GroundMesh->material.kShininess = 1.0f;
	GroundMesh->setHb(true, Position(-500, -100, -500), Position(500, 0, 500), Vector3(0, 0, 0), Vector3(0, 1, 0));
	//Set hitbox of groundmesh to be flat on XZ plane

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//frontf.tga");
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//backf.tga");
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//leftf.tga");
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//rightf.tga");
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//topf.tga");
	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.0f, 1.0f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//bottomf.tga");

	meshList[GEO_CUBE] = MeshBuilder::GenerateOBJ("Vart Car", "OBJ//VartCar.obj");
	meshList[GEO_CUBE]->textureID = LoadTGA("Image//CarUV.tga");



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


	camera = new Camera3;
	camera->Init(Vector3(0, 0, 7), Vector3(0, 0, 0), Vector3(0, 1, 0));

	//	player->getInstance();
	//	player->getInstance()->init();
	//	camera = new A3Cam(player);

	//	initEnvironment();
	//	car->getInstance();

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


void Test::Update(double dt)
{

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



	double c_posx, c_posy;
	glfwGetCursorPos(Application::m_window, &c_posx, &c_posy);
	int width, height;
	glfwGetWindowSize(Application::m_window, &width, &height);

	glfwSetCursorPos(Application::m_window, width / 2, height / 2);

	double dx, dy;
	dx = dt * double(width / 2 - c_posx);
	dy = dt * double(height / 2 - c_posy);
	camera->Update(dt, dx, dy);

	FramesPerSec = 1 / dt;
}

void Test::Render()
{
	// Render VBO here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera->getPosition().x, camera->getPosition().y, camera->getPosition().z,
		camera->getPosition().x + camera->getDir().x, camera->getPosition().y + camera->getDir().y, camera->getPosition().z + camera->getDir().z,
		camera->getUp().x, camera->getUp().y, camera->getUp().z);
	modelStack.LoadIdentity();




	RenderMesh(meshList[GEO_AXES], false);



	RenderA3Skybox();
	//	renderEnvironment();

}

void Test::Exit()
{
	for (size_t i = 0; i < GEOMETRY_TYPE::NUM_GEOMETRY; i++)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	delete GroundMesh;
	delete camera;
	
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
void Test::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	if (mesh->textureID > 0)
	{
		glUniform1i(Test::m_parameters[Test::UNIFORM_TYPE::U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(Test::m_parameters[Test::UNIFORM_TYPE::U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(Test::m_parameters[Test::UNIFORM_TYPE::U_COLOR_TEXTURE_ENABLED], 0);
	}


	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(Test::m_parameters[Test::UNIFORM_TYPE::U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(Test::m_parameters[Test::UNIFORM_TYPE::U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{

		glUniform1i(Test::m_parameters[Test::UNIFORM_TYPE::U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(Test::m_parameters[Test::UNIFORM_TYPE::U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(Test::m_parameters[Test::UNIFORM_TYPE::U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(Test::m_parameters[Test::UNIFORM_TYPE::U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(Test::m_parameters[Test::UNIFORM_TYPE::U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(Test::m_parameters[Test::UNIFORM_TYPE::U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(Test::m_parameters[UNIFORM_TYPE::U_LIGHTENABLED], 0);
	}
	mesh->Render();
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
void Test::RenderA3Skybox()
{
	//modelStack.PushMatrix();
	//modelStack.Translate(0, 0, -248);
	//modelStack.Scale(500.0f, 500.0f, 500.0f);
	//RenderMesh(meshList[GEO_FRONT], false);
	//modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(0, 0, 248);
	//modelStack.Rotate(180.0f, 0, 1, 0);
	//modelStack.Scale(500.0f, 500.0f, 500.0f);
	//RenderMesh(meshList[GEO_BACK], false);
	//modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(-248, 0, 0);
	//modelStack.Rotate(90.0f, 0, 1, 0);
	//modelStack.Scale(500.0f, 500.0f, 500.0f);
	//RenderMesh(meshList[GEO_LEFT], false);
	//modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(248, 0, 0);
	//modelStack.Rotate(-90.0f, 0, 1, 0);
	//modelStack.Scale(500.0f, 500.0f, 500.0f);
	//RenderMesh(meshList[GEO_RIGHT], false);
	//modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(0, 248, 0);
	////modelStack.Rotate(-90.0f, 0, 1, 0);
	//modelStack.Rotate(90.0f, 1, 0, 0);
	//modelStack.Scale(500.0f, 500.0f, 500.0f);
	//RenderMesh(meshList[GEO_TOP], false);
	//modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(0, -248, 0);
	////modelStack.Rotate(-90.0f, 0, 1, 0);
	//modelStack.Rotate(-90.0f, 1, 0, 0);
	//modelStack.Scale(500.0f, 500.0f, 500.0f);
	//RenderMesh(meshList[GEO_BOTTOM], false);
	//modelStack.PopMatrix();


}
void Test::RenderText(Text_Data* TextData, std::string text, Color color)
{
	if (!TextData->Text_Mesh || TextData->Text_Mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextData->Text_Mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	float nextCharWidth = 0.0f;
	for (unsigned i = 0; i < text.length(); ++i)
	{
		char Character = text[i];
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(nextCharWidth, 0, 0); //1.0f is the spacing of each character, you may change this value
		nextCharWidth += TextData->TextWidth[Character];
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		TextData->Text_Mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}
void Test::RenderTextOnScreen(Text_Data* TextData, std::string text, Color color, float size, float x, float y)
{
	if (!TextData->Text_Mesh || TextData->Text_Mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Scale(size, size, size);
	modelStack.Translate(x, y, 0);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextData->Text_Mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	float nextCharWidth = 0.0f;
	for (unsigned i = 0; i < text.length(); ++i)
	{
		char Character = text[i];
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(nextCharWidth, 0, 0); //1.0f is the spacing of each character, you may change this value
		nextCharWidth += TextData->TextWidth[Character];
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		TextData->Text_Mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
}
void Test::RenderMeshOnScreen(Mesh* mesh, int x, int y, int sizex, int sizey)
{
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	//to do: scale and translate accordingly

	modelStack.Translate(x, y, 0);
	modelStack.Scale(sizex, sizey, 1);
	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}


