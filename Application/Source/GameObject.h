#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include <string>

class GameObject abstract
{
protected:
	GameObject(std::string name) { name_ = name; };
	~GameObject() {};
	std::string name_; // Name of the object
};

#endif