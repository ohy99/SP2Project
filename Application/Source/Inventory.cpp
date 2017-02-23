#include "Inventory.h"
#include "RenderMesh.h"
//#include <algorithm>
#include "Application.h"
#include "MeshBuilder.h"
#include "UI.h"
#include <vector>

#include "Vector3.h"

std::vector<Vector3> Inventory_;
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
	for (size_t i = 0; i < Storage.size(); i++)
	{
		if (Storage[i] == NULL)
			Storage.push_back(items);

		else
			isInventoryFull = true;
	}
}

int Inventory::getInventorySize()
{
	return Storage.size();
}

void Inventory::Init()
{
	isInventory = false;
	wasIPressed = false;
	wasLeftMouseButtonPressed = false;
	meshList[INVENTORY] = MeshBuilder::GenerateQuad("Inventory", Color(1.f, 1.f, 1.f), 1.f, 1.f);
	meshList[INVENTORY]->textureID = LoadTGA("Image//inventory.tga");
	meshList[SLOTS] = MeshBuilder::GenerateQuad("Slots", Color(1.f, 0.f, 0.f), 1.f, 1.f);
	meshList[SLOTS]->textureID = LoadTGA("Image//empty_slot.tga");

	min = Vector3(-50.f, -50.f, 0.f);
	max = Vector3(50.f, 50.f, 2.f);
	y = 600;

	for (int i = 0; i < 4; i++)
	{
		x = 675;

		for (int j = 0; j < 3; j++)
		{
			Mesh* temp_Slot = MeshBuilder::GenerateQuad("Slots", Color(1.f, 0.f, 0.f), 1.f, 1.f);
			Inventory_.push_back(Vector3(x, y, 0.f));


			pos = Vector3((float)(Application::getWindowWidth() / 1024.f) * x, (float)(Application::getWindowHeight() / 768.f) * y, 0.f);
			temp_Slot->setHb(true, min, max, pos, Vector3(0.f, 1.f, 0.f));
			Slots.push_back(temp_Slot);

			x += 125;
		}

		y -= 150;
	}
}

void Inventory::Update(double dt)
{
	glfwGetCursorPos(Application::m_window, &cur_x, &cur_y);
	isIPressed = Application::IsKeyPressed('I');
	leftButton = glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT);
	isLeftMouseButtonPressed = leftButton;

	if (isLeftMouseButtonPressed && !wasLeftMouseButtonPressed)
	{
		for (size_t i = 0; i < Inventory_.size(); i++)
		{
			if (Slots[i]->isPointInsideAABB(Position(cur_x, Application::getWindowHeight() - cur_y, 0.f)))
				slotPosition = i;		
		}
		wasLeftMouseButtonPressed = isLeftMouseButtonPressed;
	}

	if (!isLeftMouseButtonPressed && wasLeftMouseButtonPressed)
		wasLeftMouseButtonPressed = isLeftMouseButtonPressed;

	if (isIPressed && !wasIPressed)
	{
		if (!isInventory && !UI::getInstance()->isPauseOpen())
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
	if (isInventoryFull)
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], "Your inventory is full", Color(1.f, 1.f, 1.f), 10, 10, 26, projectionStack, viewStack, modelStack, m_parameters);
}

void Inventory::Render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters, Mesh* itemTexture)
{
	if (isInventory)
	{
		RenderMeshClass::RenderMeshOnScreen(meshList[INVENTORY], 800, 375, 0, 400, 700, projectionStack, viewStack, modelStack, m_parameters);

		for (size_t i = 0; i < Inventory_.size(); i++)
		{
			//meshList[SLOTS] = itemTexture;
			RenderMeshClass::RenderMeshOnScreen(meshList[SLOTS], Inventory_[i].x, Inventory_[i].y, Inventory_[i].z, 100, 100, projectionStack, viewStack, modelStack, m_parameters);
		}
	}
}

bool Inventory::isInventoryOpen()
{
	if (!isInventory)
		return false;

	else
		return true;
}