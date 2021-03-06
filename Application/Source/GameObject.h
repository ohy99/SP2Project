#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include <string>
#include "Mesh.h"
//#include "SceneManager.h"

class GameObject abstract
{
protected:
	GameObject(const std::string& name) : kName(name) { /*SceneManager::getInstance()->getCurrentScene()->AddObjToCurrentScene(this);*/ };
	const std::string kName; // Name of the object
	unsigned ObjID;
	std::string ObjName;
	std::string TextureName;
public: 
	virtual ~GameObject() { delete CollisionMesh_; };
	std::string getName(void) {	return kName; }
	Mesh* CollisionMesh_ = 0;
};

#endif