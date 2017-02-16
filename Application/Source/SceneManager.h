#ifndef _SCENEMANAGER_H
#define _SCENEMANAGER_H

#include <vector>
#include "Scene.h"
#include "Application.h"

class SceneManager
{
	friend class Scene;
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
	void SetNextScene() { currSceneID = nextSceneID; }
	static Scene* getCurrentScene() { return scenes.at(currSceneID); }
	void Update() {

		scenes.at(currSceneID)->Update(Application::m_timer.getElapsedTime());
		scenes.at(currSceneID)->Render();
	}
};


#endif

