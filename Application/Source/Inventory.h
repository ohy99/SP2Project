#ifndef _INVENTORY_H
#define _INVENTORY_H

#include "Item.h"
#include "MatrixStack.h"
#include <vector>

class Inventory
{
public:
	virtual ~Inventory() { ; }



	// Getter
	Item* getItem(int i); // Be able to get item from inventory

	int getInventorySize(); // Get the size of the inventory

	// Setter
	void setItem(Item* items); // Put item from hand to inventory || from the ground to inventory

	void Init();
	void Update(double dt);
	void Render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters); // use to render Inventory onto the screen

	bool isInventoryOpen();
	static Inventory* getInstance();

private:
	enum GEOMETRY_TYPE
	{
		INVENTORY,
		EMPTY_SLOTS,

		NUM_GEOMETRY,
	};

	static Inventory* Instance_;
	Mesh* meshList[NUM_GEOMETRY];

	Inventory(const std::string& name) : isInventoryFull(false), currItem(NULL) { ; }

	bool isInventoryFull; // render message if inventory is full
	Item* currItem;

	int width, height;
	int leftButton;
	bool isLeftMouseButtonPressed, wasLeftMouseButtonPressed;
	bool isIPressed, wasIPressed, isInventory;

	int x, y;
	int size;
	int slotPosition;

	Vector3 min, max, pos;
	double cur_x, cur_y;
};

struct InventorySlot
{
	Mesh* hitBox; // to store the position of the slots
	Item* item_; // to store the item onto the slot
};

#endif