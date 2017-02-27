#ifndef InsideBarrackScene_H
#define InsideBarrackScene_H

#include "Scene.h"
#include "SceneManager.h"

#include "Camera.h"
#include "Camera3.h"
#include "Mesh.h"
//#include "MeshBuilder.h"
#include "MatrixStack.h"
#include "Light.h"
#include "Teleporter.h"
#include <vector>

#include "Player.h"
//#include "MainMenu.h"
#include "UI.h"
#include "NPC.h"
#include "Environment.h"


class InsideBarrackScene : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_QUAD,
		GEO_CUBE,
		GEO_CIRCLE,
		GEO_RING,
		GEO_SPHERE,
		GEO_HEMISPHERE,
		GEO_TORUS,
		GEO_CYLINDER,
		GEO_LIGHTBALL,
		GEO_LIGHTBALL1,
		GEO_GroundMesh_RedDirt,
		GEO_DOOR,

		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,

		NUM_GEOMETRY,
	};

public:
	InsideBarrackScene();
	~InsideBarrackScene();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	//virtual std::vector<GameObject*> getCurrentSceneObjs(){ return Game_Objects_; }

	static MS modelStack, viewStack, projectionStack;

	//static EnvironmentObj* Teleporter;
	static Teleporter* Barrack;

private:
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];

	//unsigned m_programID;
	//unsigned m_parameters[U_TOTAL];
	/*MS modelStack, viewStack, projectionStack;*/

	double FramesPerSec;

	Camera *camera;

	void RenderSkybox();

	Light light[8];

	static std::vector<EnvironmentObj*> Env_Obj;

	//static std::vector<NPC*> CampNPC;
	//std::vector

	bool isPause;
	bool isEscPressed, wasEscPressed;

	//MainScreen MainMenu;
	//UI renderUI;

	void Interactions();

	double x, y;
	//void RenderText(Text_Data* TextData, std::string text, Color color);
	//void RenderTextOnScreen(Text_Data* TextData, std::string text, Color color, float size, float x, float y);
	//void RenderMeshOnScreen(Mesh* mesh, int x, int y, int sizex, int sizey);

};


#endif