#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include "Scene.h"
#include "SceneManager.h"

#include "Camera.h"
#include "Camera3.h"
#include "MatrixStack.h"

#include <string>

class LoadingScreen : public Scene
{
public:
	LoadingScreen();
	~LoadingScreen();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	MS modelStack, viewStack, projectionStack;
private:
	unsigned m_vertexArrayID;
	double FramesPerSec;

	Camera *camera;
	std::string loadingText;
};

#endif