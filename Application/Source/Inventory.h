#ifndef _INVENTORY_H
#define _INVENTORY_H

#include "Item.h"
#include "MatrixStack.h"
#include <vector>

class Inventory : public Item
{
public:
	virtual ~Inventory() { ; }

	// Getter
	Item* getItem(int i); // Be able to get item from inventory

	int getInventorySize(); // Get the size of the inventory

	// Setter
	void setItem(Item* items); // Put item from hand to inventory || from the ground to inventory

	void renderMessage(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters); // Message to tell player that inventory is full
	static Inventory* getInstance();

private:
	Inventory(const std::string& name) : Item(name), maxStorage(9), isInventoryFull(false), currItem(NULL)
	{
		for (int i = 0; i < 9; i++)
			Storage.push_back(NULL);
	}

	std::vector<Item*> Storage;
	int maxStorage; // defalut set it to 10, increase gradually by 4 every upgrade
	bool isInventoryFull; // render message if inventory is full
	Item* currItem;

	static Inventory* Instance_;
};

#endif