#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H

#include "Scene.h"
#include "SceneManager.h"

#include "Camera.h"
#include "Camera3.h"
#include "MatrixStack.h"

#include <string>

class SplashScreen : public Scene
{
public:
	SplashScreen();
	~SplashScreen();

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
	bool goThroughRenderOnce;
};

#endif