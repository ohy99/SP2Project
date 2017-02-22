#ifndef _TELEPORTER_H
#define _TELEPORTER_H

#include "Environment.h"

class Teleporter : public EnvironmentObj
{
	int nextSceneID;
public:
	Teleporter(Mesh* obj, int nextSceneID) : EnvironmentObj(obj), nextSceneID(nextSceneID)
	{}
	~Teleporter(){};
	int getNextSceneID(){ return nextSceneID; }
};

#endif