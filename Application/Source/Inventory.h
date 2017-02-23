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
	void renderMessage(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters); // Message to tell player that inventory is full

	bool isInventoryOpen();
	static Inventory* getInstance();

private:
	enum GEOMETRY_TYPE
	{
		INVENTORY,
		SLOTS,
		//SLOT2,
		//SLOT3,
		//SLOT4,
		//SLOT5,
		//SLOT6,
		//SLOT7,
		//SLOT8,
		//SLOT9,
		//SLOT10,
		//SLOT11,
		//SLOT12,

		NUM_GEOMETRY,
	};

	static Inventory* Instance_;
	Mesh* meshList[NUM_GEOMETRY];

	std::vector<Mesh*> Slots;

	Inventory(const std::string& name) : maxStorage(9), isInventoryFull(false), currItem(NULL)
	{
		for (int i = 0; i < 9; i++)
			Storage.push_back(NULL);
	}

	std::vector<Item*> Storage;
	int maxStorage; // defalut set it to 10, increase gradually by 4 every upgrade
	bool isInventoryFull; // render message if inventory is full
	Item* currItem;

	int width, height;
	int leftButton;
	bool isLeftMouseButtonPressed, wasLeftMouseButtonPressed;
	bool isIPressed, wasIPressed, isInventory;

	int x, y;
	int slotPosition;

	Vector3 min, max, pos;
	double cur_x, cur_y;
};

//struct InventorySlot
//{
//	Vector3 position; // to store the position of the slots
//	Item* item_; // to store the item onto the slot
//};

#endif