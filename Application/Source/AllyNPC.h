#ifndef _NONENEMY_H
#define _NONENEMY_H

#include "NPC.h"

class AllyNPC abstract : public NPC
{
	
public:
	AllyNPC(std::string name) : NPC(name){};
	virtual ~AllyNPC(){};
	virtual void update(double dt){};
	virtual void render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters) {};
};

#endif