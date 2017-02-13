#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include <string>

class GameObject abstract
{
protected:
	GameObject(const std::string& name) : kName(name) {};
	const std::string kName; // Name of the object

public: 
	virtual ~GameObject() {};
	std::string getName(void);
};

#endif