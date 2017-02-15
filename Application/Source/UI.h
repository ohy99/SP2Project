#ifndef UI_H
#define UI_H

#include "MainScene.h"

class UI
{
public:
	UI() { ; }
	~UI() { ; }

	void Init();
	void renderPause();

private:
	enum GEOMETRY_TYPE
	{
		QUAD, 

		NUM_GEOMETRY,
	};

	Mesh* meshList[NUM_GEOMETRY];
	MainScene userInterface;
};

#endif