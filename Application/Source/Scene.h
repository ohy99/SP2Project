#ifndef SCENE_H
#define SCENE_H

#include <vector>
//#include "GameObject.h"
#include "Mesh.h"
#include "LoadTextData.h"
#include "LoadTGA.h"

class Scene abstract
{
	//friend class GameObject;
public:
	Scene() {}
	~Scene() {		
		//for (size_t i = 0; i < Game_Objects_.size(); ++i)
		//{
		//	Game_Objects_.pop_back();
		//}
	}

	virtual void Init() = 0;
	virtual void Update(double dt) = 0;
	virtual void Render() = 0;
	virtual void Exit() = 0;

	static enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,

		U_LIGHTENABLED,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,

		U_NUMLIGHTS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,

		U_TEXT_ENABLED,
		U_TEXT_COLOR,

		U_TOTAL,
	};
	static unsigned m_parameters[Scene::UNIFORM_TYPE::U_TOTAL];

	static enum TEXT_TYPE
	{
		Calibri,
		Century,
		Chiller,
		ScriptMTBold,
		SegoeMarker,
		Text_Count
	};

	static struct Text_Data
	{
		Mesh* Text_Mesh = 0;
		float TextWidth[256];
	};
	static Text_Data Text[TEXT_TYPE::Text_Count];
	static unsigned m_programID;

	//virtual void AddObjToCurrentScene(GameObject* newGObj) { Game_Objects_.push_back(newGObj); }
protected:
	//std::vector<GameObject*> Game_Objects_;
	//virtual std::vector<GameObject*> getCurrentSceneObjs(){ return Game_Objects_; }
	static bool debugMode;
};



#endif

