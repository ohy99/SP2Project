#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include "Scene.h"
#include "SceneManager.h"

#include "Camera.h"
#include "Camera3.h"
#include "Mesh.h"
#include "MatrixStack.h"

#include <vector>


class MainScreen : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_QUAD,
		MAIN_SCREEN,
		START_BUTTON,
		QUIT,

		NUM_GEOMETRY,
	};

public:
	MainScreen();
	~MainScreen();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	MS modelStack, viewStack, projectionStack;
private:
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	double FramesPerSec;

	Camera *camera;

	Vector3 min, max, pos;

	double x, y;
	int leftButton;
	bool isStartPressed;
	bool isLeftMouseButtonPressed, wasLeftMouseButtonPressed;
};

#endif