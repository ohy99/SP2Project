#include "UI.h"
#include "MeshBuilder.h"
#include "RenderMesh.h"
#include "Application.h"
#include "Inventory.h"
#include "MGPlayer.h"
#include "SceneManager.h"
#include "MiniGame.h"

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
	count[PAUSE] = MeshBuilder::GenerateQuad("image", Color(0.9f, 0.9f, 0.9f), 1.0f, 1.0f);
	count[PAUSE]->textureID = LoadTGA("Image//pause.tga");

	count[RESUME] = MeshBuilder::GenerateQuad("image", Color(0.9f, 0.9f, 0.9f), 1.0f, 1.0f);
	count[RESUME]->setHb(true, Vector3((float)(Application::getWindowWidth() / 1024.f) * -125.f, (float)(Application::getWindowHeight() / 768.f) * -50.f, 0.f),
		Vector3((float)(Application::getWindowWidth() / 1024.f) * 125.f, (float)(Application::getWindowHeight() / 768.f) * 50.f, 2.f),
		Vector3((float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 768.f) * 450.f, 0.f),
		Vector3(0.f, 1.f, 0.f));

	count[QUIT] = MeshBuilder::GenerateQuad("image", Color(0.9f, 0.9f, 0.9f), 1.0f, 1.0f);
	count[QUIT]->setHb(true, Vector3((float)(Application::getWindowWidth() / 1024.f) * -175.f, (float)(Application::getWindowHeight() / 768.f) * -50.f, 0.f),
		Vector3((float)(Application::getWindowWidth() / 1024.f) * 175.f, (float)(Application::getWindowHeight() / 768.f) * 50.f, 2.f),
		Vector3((float)Application::getWindowWidth() * 0.5f, (float)(Application::getWindowHeight() / 768.f) * 275.f, 0.f),
		Vector3(0.f, 1.f, 0.f));

	wasLeftMouseButtonPressed = false;
}

void UI::Update(double dt)
{
	glfwGetCursorPos(Application::m_window, &cur_x, &cur_y);
	isIPressed = Application::IsKeyPressed('I');
	leftButton = glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT);
	isLeftMouseButtonPressed = leftButton;
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

	if (isLeftMouseButtonPressed && !wasLeftMouseButtonPressed)
	{
		if (count[RESUME]->isPointInsideAABB(Position(cur_x, Application::getWindowHeight() - cur_y, 0.f)))
		{
			isPause = false;
			glfwSetInputMode(Application::m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

/*		if (count[QUIT]->isPointInsideAABB(Position(cur_x, Application::getWindowHeight() - cur_y, 0.f)) && MGPlayer::getInstance()->gameStarted())
		{
			MiniGame::Reset();
			MGPlayer::getInstance()->setGameState(false, false, false);
			SceneManager::getInstance()->SetNextSceneID(1);
			SceneManager::getInstance()->SetNextScene();
		}

		else*/ if (count[QUIT]->isPointInsideAABB(Position(cur_x, Application::getWindowHeight() - cur_y, 0.f)))
			Application::setCloseWindow(true);

		wasLeftMouseButtonPressed = isLeftMouseButtonPressed;
	}

	if (!isLeftMouseButtonPressed && wasLeftMouseButtonPressed)
		wasLeftMouseButtonPressed = isLeftMouseButtonPressed;

	if (count[RESUME]->isPointInsideAABB(Position(cur_x, Application::getWindowHeight() - cur_y, 0.f)))
		count[RESUME]->textureID = LoadTGA("Image//pauseresume2.tga");
	else
		count[RESUME]->textureID = LoadTGA("Image//pauseresume.tga");

	if (count[QUIT]->isPointInsideAABB(Position(cur_x, Application::getWindowHeight() - cur_y, 0.f)))
		count[QUIT]->textureID = LoadTGA("Image//pausequit2.tga");
	else
		count[QUIT]->textureID = LoadTGA("Image//pausequit.tga");

	if (!isEscPressed && wasEscPressed) // When you release the ESC button
		wasEscPressed = isEscPressed;
}

void UI::renderPause(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	if (isPause)
	{
		RenderMeshClass::RenderMeshOnScreen(count[PAUSE], (int)Application::getWindowWidth() * 0.5, (int)Application::getWindowHeight() * 0.5, 0, (int)(Application::getWindowWidth() / 1024) * 700, (int)(Application::getWindowHeight() / 768) * 700, projectionStack, viewStack, modelStack, m_parameters);
		RenderMeshClass::RenderMeshOnScreen(count[RESUME], (int)Application::getWindowWidth() * 0.5, (int)(Application::getWindowHeight() / 768) * 450, 0, (int)(Application::getWindowWidth() / 1024) * 250, (int)(Application::getWindowHeight() / 768) * 100, projectionStack, viewStack, modelStack, m_parameters);
		RenderMeshClass::RenderMeshOnScreen(count[QUIT], (int)Application::getWindowWidth() * 0.5, (int)(Application::getWindowHeight() / 768) * 275, 0, (int)(Application::getWindowWidth() / 1024) * 350, (int)(Application::getWindowHeight() / 768) * 100, projectionStack, viewStack, modelStack, m_parameters);
	}
}

bool UI::isPauseOpen()
{
	return isPause;
}