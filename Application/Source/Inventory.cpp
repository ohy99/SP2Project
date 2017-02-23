#include "Inventory.h"
#include "RenderMesh.h"
//#include <algorithm>
#include "Application.h"
#include "MeshBuilder.h"

Inventory* Inventory::Instance_ = NULL;

Inventory* Inventory::getInstance()
{
	if (Instance_)
		return Instance_;
	else
		return (Instance_ = new Inventory("Inventory"));
}

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

void Inventory::Init()
{
	isInventory = false;
	meshList[INVENTORY] = MeshBuilder::GenerateQuad("Inventory", Color(1.f, 1.f, 1.f), 1.f, 1.f);
}

void Inventory::Update(double dt)
{
	isIPressed = Application::IsKeyPressed('I');

	if (isIPressed && !wasIPressed)
	{
		if (!isInventory)
		{
			isInventory = true;
			glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		else
			isInventory = false;

		wasIPressed = isIPressed;
	}

	if (!isIPressed && wasIPressed)
		wasIPressed = isIPressed;
}

void Inventory::renderMessage(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	if (Application::IsKeyPressed('F') && isInventoryFull)
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], "Your inventory is full", Color(1.f, 1.f, 1.f), 2, 35, 26, projectionStack, viewStack, modelStack, m_parameters);
}

void Inventory::Render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	if (isInventory)
		RenderMeshClass::RenderMeshOnScreen(meshList[INVENTORY], 500, 200, 0, 500, 300, projectionStack, viewStack, modelStack, m_parameters);
}

bool Inventory::isInventoryOpen()
{
	if (!isInventory)
		return false;

	else
		return true;
}