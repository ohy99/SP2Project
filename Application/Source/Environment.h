#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

#include "GameObject.h"
#include "Mesh.h"

class EnvironmentObj : public GameObject
{
public:
	EnvironmentObj(Mesh* obj) : GameObject(obj->name) { CollisionMesh_ = obj; CollisionMesh_->collisionEnabled = true; };
	~EnvironmentObj() {};
};

#endif