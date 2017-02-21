#ifndef MINI_GAME_H
#define MINI_GAME_H

#include "Scene.h"
#include "SceneManager.h"

#include "Camera.h"
#include "CameraMG.h"
#include "Mesh.h"
//#include "MeshBuilder.h"
#include "MatrixStack.h"
#include "Light.h"

#include <vector>

#include "MGPlayer.h"
#include "Player.h"
#include "Distance.h"
#include "MainScreen.h"
#include "Environment.h"
#include "UI.h"


class MiniGame : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_QUAD,

		GEO_LIGHTBALL,
		GEO_LIGHTBALL1,

		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,

		ROAD,
		LORRY,

		NUM_GEOMETRY,
	};

public:
	MiniGame();
	~MiniGame();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	static MS modelStack, viewStack, projectionStack;

private:
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	unsigned m_programID;
	double FramesPerSec;

	Camera *camera;

	void RenderSkybox();
	void RenderMiniGame();

	Light light[8];

	bool isPause;
	bool isEscPressed, wasEscPressed;

	UI renderUI;
	static std::vector<EnvironmentObj*> Obstacles;


	double x, y;
	float roadDistance;

	Distance roadDis;
	Mesh* road;
	Mesh* lorry;
};

#endif