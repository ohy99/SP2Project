#ifndef _SCENEMANAGER_H
#define _SCENEMANAGER_H

#include <vector>
#include "Scene.h"
#include "Application.h"

class SceneManager
{
	SceneManager() {} ;
	static SceneManager* Instance;
	std::vector<Scene*> scenes;
	int currSceneID = 0;
	int nextSceneID = 0;
public:
	static SceneManager* getInstance() { if (Instance) return Instance; else return (Instance = new SceneManager()); };
	~SceneManager() { while (scenes.size()) scenes.pop_back(); };
	void AddScene(Scene *scene) { scenes.push_back(scene); }
	void SetNextSceneID(int sceneID) { nextSceneID = sceneID; }
	void SetNextScene() { currSceneID = nextSceneID; }
	void Update() {

		scenes.at(currSceneID)->Update(Application::m_timer.getElapsedTime());
		scenes.at(currSceneID)->Render();
	}
};


#endif