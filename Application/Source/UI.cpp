#include "UI.h"
#include "MeshBuilder.h"
#include "RenderMesh.h"
#include "Application.h"
#include "Inventory.h"

UI* UI::Instance_ = NULL;

UI* UI::getInstance()
{
	if (Instance_)
		return Instance_;
	else
		return (Instance_ = new UI());
}

void UI::Init()
{
	isPause = false;
	wasEscPressed = false;
	Quad = MeshBuilder::GenerateQuad("image", Color(0.9f, 0.9f, 0.9f), 1.0f, 1.0f);
}

void UI::Update(double dt)
{
	isEscPressed = Application::IsKeyPressed(VK_ESCAPE);

	if (isEscPressed && !wasEscPressed) // When you press ESC
	{
		if (!isPause && !Inventory::getInstance()->isInventoryOpen())
		{
			isPause = true;
			glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			isPause = false;
			glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		wasEscPressed = isEscPressed;
	}

	if (!isEscPressed && wasEscPressed) // When you release the ESC button
		wasEscPressed = isEscPressed;
}

void UI::renderPause(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	if (isPause)
		RenderMeshClass::RenderMeshOnScreen(Quad, (int)Application::getWindowWidth() * 0.5, (int)Application::getWindowHeight() * 0.5, 0, (int)(Application::getWindowWidth() / 1024) * 700, (int)(Application::getWindowHeight() / 768) * 700, projectionStack, viewStack, modelStack, m_parameters);
}

bool UI::isPauseOpen()
{
	if (!isPause)
		return false;

	else
		return true;
}