#ifndef _NPC_H
#define _NPC_H

#include "GameObject.h"

class NPC abstract : public GameObject
{
public:
	virtual void update(double dt);
	virtual void render();
};

#endif