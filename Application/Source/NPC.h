#ifndef _NPC_H
#define _NPC_H

#include "GameObject.h"
#include "MatrixStack.h"

class NPC abstract : public GameObject
{
public:
	NPC(std::string name) : GameObject(name) {};
	virtual ~NPC() {};
	virtual void update(double dt){};
	virtual void render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters) {};
};

#endif