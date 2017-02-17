#ifndef MINI_GAME_H
#define MINI_GAME_H

#include "Scene.h"
#include "SceneManager.h"

#include "Camera.h"
#include "Camera3.h"
#include "Mesh.h"
//#include "MeshBuilder.h"
#include "MatrixStack.h"
#include "Light.h"

#include <vector>

#include "Player.h"
#include "MainScreen.h"
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

	Light light[8];

	bool isPause;
	bool isEscPressed, wasEscPressed;

	UI renderUI;

	double x, y;

};

#endif