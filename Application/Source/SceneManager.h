#ifndef _SCENEMANAGER_H
#define _SCENEMANAGER_H

#include <vector>
#include "Scene.h"
#include "Application.h"

//#include "MainScene.h"
//#include "BossScene.h"
//#include "MainMenu.h"
//#include "UndergroundScene.h"
//#include "MiniGame.h"
//#include "WorldScene.h"
//#include "InsideBarrackScene.h"
//#include "SplashScreen.h"

class SceneManager
{
	//friend class Scene;
	friend class Application;
	SceneManager() {} ;
	static SceneManager* Instance;
	static std::vector<Scene*> scenes;
	static int currSceneID;
	static int nextSceneID;

public:
	static SceneManager* getInstance() { if (Instance) return Instance; else return (Instance = new SceneManager()); };
	~SceneManager() { while (scenes.size()) scenes.pop_back(); };
	void AddScene(Scene *scene) { scenes.push_back(scene); }

	void SetNextSceneID(int sceneID) { nextSceneID = sceneID; }
	void SetNextScene() {
		scenes.at(currSceneID)->Exit(); 
		if (currSceneID)//if its not in splash screen
		{
			currSceneID = SCENES::LOADINGSCREEN;
			scenes.at(currSceneID)->Init();
			Update();
			scenes.at(currSceneID)->Exit();
		}
		scenes.at(nextSceneID)->Init(); 
		currSceneID = nextSceneID;
	}
	static Scene* getCurrentScene() { return scenes.at(currSceneID); }
	void Update() {
		scenes.at(currSceneID)->Update(Application::m_timer.getElapsedTime());
		scenes.at(currSceneID)->Render();
		//Swap buffers
		glfwSwapBuffers(Application::m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
		Application::m_timer.waitUntil(Application::frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.   
	}


	static enum SCENES
	{
		SPLASHSCREEN = 0,
		MAINMENU,
		CAMPSCENE,
		WORLDSCENE,
		BARRACKSCENE,
		BOSSSCENE,
		UNDERGROUNDSCENE,
		MINIGAMESCENE,
		LOADINGSCREEN,
		SCENE_COUNT
	};
};


#endif

