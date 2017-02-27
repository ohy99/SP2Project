#ifndef MINI_MAP_H
#define MINI_MAP_H

#include "MatrixStack.h"
#include "Mesh.h"

class MiniMap
{
public:
	virtual ~MiniMap();

	static MiniMap* getInstance();

	void Init();
	void Update(double dt);
	void Render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);

	//Setter
	void setCurrentMiniMap(Mesh* miniMap); // Render mini map on screen

private:
	MiniMap();
	static MiniMap* Instance_;

};

#endif