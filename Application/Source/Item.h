#ifndef ITEM_H
#define ITEM_H

#include "GameObject.h"
#include "Mesh.h"

class Item : public GameObject
{
public:
	Item(const std::string& name) : GameObject(name) {}
	virtual ~Item() {};


	enum TEXTURE_ID // only to pass in these for rendering different texture onto the inventory
	{
		ITEM_TEXTURE,

		NUM_TEXTURE,
	};

	Mesh* texture[NUM_TEXTURE];
};

#endif