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

		//GEO_LIGHTBALL,
		//GEO_LIGHTBALL1,

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

	enum MINI_GAME_UI
	{
		SCREEN,
		START,
		RESTART,
		QUIT,

		MG_UI_COUNT,
	};

public:
	MiniGame();
	~MiniGame();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	virtual void Reset();

	virtual void GameState();

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

	static std::vector<EnvironmentObj*> Obstacles;

	int width, height;

	char order[2]; // rand the order of lorry, 0 & 1
	char counter;

	double x, y;
	double c_posx, c_posy;

	float score;
	float bonusScore;
	float roadDistance;
	float obstaclePosX, obstaclePosZ;

	int leftButton;
	bool isLeftMouseButtonPressed, wasLeftMouseButtonPressed;

	Mesh* MGList[MG_UI_COUNT];

	std::string Score;

	Mesh* road;
	EnvironmentObj* lorry;
};

#endif