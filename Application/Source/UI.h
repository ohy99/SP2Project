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
	UI() { ; }

	enum GEOMETRY_TYPE
	{
		QUAD, 

		NUM_GEOMETRY,
	};

	Mesh* meshList[NUM_GEOMETRY];

	bool isPause;
	bool isEscPressed, wasEscPressed;
	int width, height;

	static UI* Instance_;
};

#endif