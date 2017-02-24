#ifndef ITEM_H
#define ITEM_H

#include "GameObject.h"
#include "Mesh.h"

class Item : public GameObject
{
public:
	Item(const std::string& name) : GameObject(name), isItemInInventory(false) { ; }
	virtual ~Item() {};

	Mesh* item2DTexture;
	bool isItemInInventory;
};

#endif