#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include <string>
#include "Mesh.h"


class GameObject abstract
{
protected:
	GameObject(const std::string& name) : kName(name) { /*MainScene::Game_Objects_.push_back(this);*/ };
	const std::string kName; // Name of the object
public: 
	virtual ~GameObject() { 
		//std::vector<GameObject*>::iterator it;
		//it = std::find(MainScene::Game_Objects_.begin(), MainScene::Game_Objects_.end(), this);
		//MainScene::Game_Objects_.erase(it);
	};
	std::string getName(void)
	{
		return kName;
	}
	Mesh* CollisionMesh_;
};

#endif