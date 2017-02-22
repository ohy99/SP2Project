#include "Inventory.h"
#include "RenderMesh.h"
#include <algorithm>

Inventory* Inventory::Instance_ = NULL;

Item* Inventory::getItem(int i)
{
	if (Storage.size() > 0)
	{
		currItem = Storage[i];
		Storage[i] = NULL;

		return currItem;
	}

	else
		return currItem;
}

Inventory* Inventory::getInstance()
{
	if (Instance_)
		return Instance_;
	else
		return (Instance_ = new Inventory("Inventory"));
}

void Inventory::setItem(Item* items)
{
	if (Storage.size() == 0)
		Storage.push_back(items);

	else
	{
		for (size_t i = 0; i < Storage.size(); i++)
		{
			if (Storage[i] == NULL)
				Storage.push_back(items);

			else
				isInventoryFull = true;
		}
	}
}

int Inventory::getInventorySize()
{
	return Storage.size();
}

void Inventory::renderMessage(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	if (isInventoryFull)
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], "Your inventory is full", Color(1.f, 1.f, 1.f), 2, 35, 26, projectionStack, viewStack, modelStack, m_parameters);
}