#ifndef UI_H
#define UI_H

#include "Mesh.h"
#include "MatrixStack.h"

class UI
{
public:
	UI() { ; }
	~UI() { ; }

	void Init();
	void renderPause(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);

private:
	enum GEOMETRY_TYPE
	{
		QUAD, 

		NUM_GEOMETRY,
	};

	Mesh* meshList[NUM_GEOMETRY];
};

#endif