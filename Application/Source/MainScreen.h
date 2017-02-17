#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include "Mesh.h"
#include "Application.h"

#include "MatrixStack.h"
#include "Scene.h"

class MainScreen : public Scene
{
	enum GEOMETRY_TYPE
	{
		MAIN_SCREEN,
		START_BUTTON,

		NUM_GEOMETRY,
	};

public:
	MainScreen();
	~MainScreen();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit() {};

	Mesh* startButton;
	MS modelStack, viewStack, projectionStack;
	bool isMainMenu;

private:
	Mesh* meshList[NUM_GEOMETRY];

	int leftButton;
	bool isLeftMouseButtonPressed, wasLeftMouseButtonPressed;

	//void RenderText(Text_Data* TextData, std::string text, Color color);
	//void RenderTextOnScreen(Text_Data* TextData, std::string text, Color color, float size, float x, float y);
	//void RenderMeshOnScreen(Mesh* mesh, int x, int y, int sizex, int sizey);

};

#endif