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

//WorldScene::Text_Data WorldScene::Text[TEXT_TYPE::Text_Count];
//unsigned WorldScene::m_parameters[U_TOTAL];
MS WorldScene::modelStack, WorldScene::viewStack, WorldScene::projectionStack;

//std::vector<GameObject*> WorldScene::Game_Objects_(10, NULL);

//std::vector<GameObject*> WorldScene::Game_Objects_(10, NULL);
//UI renderMeshOnScreen;
std::vector<EnvironmentObj*> WorldScene::Env_Obj;
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




	//Interactable Items ----------------------------------------------- START
	EnvironmentObj* Tablet = new EnvironmentObj(MeshBuilder::GenerateOBJ("Tablet", "OBJ//WorldScene//Tablet_OBJ.obj"));
	Tablet->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//InteractableItem_OffTablet_UV_Texture.tga");
	EnvironmentObj* Robot = new EnvironmentObj(MeshBuilder::GenerateOBJ("Robot", "OBJ//WorldScene//BrokenRobot_OBJ.obj"));
	Robot->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//InteractableItem_BrokenRobot_UV_Texture.tga");
	EnvironmentObj* Blueprint1 = new EnvironmentObj(MeshBuilder::GenerateOBJ("Truck", "OBJ//Blueprint1.obj"));
	Blueprint1->CollisionMesh_->textureID = LoadTGA("Image//BlueprintUV.tga");
	//EnvironmentObj* Hard_disk = new EnvironmentObj(MeshBuilder::GenerateOBJ("Hard Disk", "OBJ//WorldScene//Harddisk_OBJ.obj"));
	//Hard_disk->CollisionMesh_->textureID = LoadTGA("Image//WorldScene//InteractableItem_Harddisk_UV_Texture.tga");
	EnvironmentObj* BrokenGuard = new EnvironmentObj(MeshBuilder::GenerateOBJ("Broken Guard", "OBJ//WorldScene//DestroyedGuard.obj"));


	Env_Obj.push_back(Tablet);
	Env_Obj.push_back(Robot);
	//Env_Obj.push_back(Hard_disk);
	Env_Obj.push_back(Blueprint1);
	Env_Obj.push_back(BrokenGuard);
	//Interactable Items ----------------------------------------------- END




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

	if (Application::IsKeyPressed('1'))
	{
		SceneManager::getInstance()->SetNextSceneID(0);
		SceneManager::getInstance()->SetNextScene();
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

	//if (MainMenu.isMainMenu)
	//	MainMenu.Render();

	//else
	//{

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



		//if (isPause)
			//renderUI.renderPause(&projectionStack, &viewStack, &modelStack, m_parameters);


	//}

	//modelStack.PushMatrix();
	//RenderMeshClass::RenderMesh(meshList[GEO_Teleporter], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	//modelStack.PushMatrix();

	//modelStack.PushMatrix();
	//RenderMeshClass::RenderMesh(meshList[GEO_UNDERGROUND_DOOR], true, &projectionStack, &viewStack, &modelStack, m_parameters);
	//modelStack.PushMatrix();

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


void WorldScene::Interactions(){


	//Teleporter to Camp base -- Main Scene
	if (Player::getInstance()->getPlayerPosition().x >= 92 && Player::getInstance()->getPlayerPosition().x <= 97 && Player::getInstance()->getPlayerPosition().z <= -8 && Player::getInstance()->getPlayerPosition().z >= -15){

		RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("[Press SPACE to teleport to Camp Base.]"), Color(1, 0, 0), 2.f, 28, 30, &projectionStack, &viewStack, &modelStack, m_parameters);
	}

	if (Application::IsKeyPressed(VK_SPACE)){

		if (Player::getInstance()->getPlayerPosition().x >= 92 && Player::getInstance()->getPlayerPosition().x <= 97 && Player::getInstance()->getPlayerPosition().z <= -8 && Player::getInstance()->getPlayerPosition().z >= -15){

			SceneManager::getInstance()->SetNextSceneID(1);
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
			SceneManager::getInstance()->SetNextSceneID(5);
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

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Extracting data…*"), Color(1, 0, 0), 2.f, 10, 37, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Discovered: This tablet holds information of a machine that can"), Color(1, 0, 0), 2.f, 10, 35, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("apparently help to rejuvenate the Earth."), Color(1, 0, 0), 2.f, 10, 33, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("There are apparently three parts to the blueprints required to make this device."), Color(1, 0, 0), 2.f, 10, 31, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("One of the locations is recorded in this device. A hidden lab at somewhere"), Color(1, 0, 0), 2.f, 10, 29, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("around coordinates: -80, -80 *"), Color(1, 0, 0), 2.f, 10, 27, &projectionStack, &viewStack, &modelStack, m_parameters);

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

			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Extracting data…*"), Color(1, 0, 0), 2.f, 10, 37, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("*Discovered: This robot appears to be a research assistant "), Color(1, 0, 0), 2.f, 10, 35, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("from a time before Earth was abandoned. One registered area of interest."), Color(1, 0, 0), 2.f, 10, 33, &projectionStack, &viewStack, &modelStack, m_parameters);
			RenderMeshClass::RenderTextOnScreen(&Text[TEXT_TYPE::Century], std::string("New coordinates found: ... *"), Color(1, 0, 0), 2.f, 10, 31, &projectionStack, &viewStack, &modelStack, m_parameters);
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