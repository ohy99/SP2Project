#ifndef UI_H
#define UI_H

#include "Mesh.h"
#include "MatrixStack.h"

class UI
{
public:
	virtual ~UI() { ; }

	void Init();
	void Update(double dt);
	void renderPause(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);

	bool isPauseOpen();

	static UI* getInstance();

private:
	enum PAUSE_UI
	{
		PAUSE,
		RESUME,
		RESUME2,
		QUIT,
		QUIT2,

		PUI_COUNT,
	};

	UI() { ; }

	Mesh* count[PUI_COUNT];

	bool isPause;
	bool isEscPressed, wasEscPressed;
	int width, height;

	double cur_x, cur_y;
	int leftButton;
	bool isLeftMouseButtonPressed, wasLeftMouseButtonPressed;
	bool isIPressed, wasIPressed, isInventory;
	bool resume, quit;

	static UI* Instance_;
};

#endif