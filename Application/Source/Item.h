#ifndef ITEM_H
#define ITEM_H

#include "GameObject.h"

class Item : public GameObject
{
public:
	Item(const std::string& name) : GameObject(name) {}
	virtual ~Item() {};
};

#endif