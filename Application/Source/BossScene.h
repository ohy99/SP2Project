#ifndef BossScene_H
#define BossScene_H

#include "Scene.h"
#include "SceneManager.h"

#include "Camera.h"
#include "Camera3.h"
#include "Mesh.h"
//#include "MeshBuilder.h"
#include "MatrixStack.h"
#include "Light.h"

#include <vector>

#include "Player.h"
#include "NPC.h"
#include "Environment.h"


class BossScene : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		GROUND,

		NUM_GEOMETRY,
	};

public:
	BossScene();
	~BossScene();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	//virtual std::vector<GameObject*> getCurrentSceneObjs(){ return Game_Objects_; }

	static MS modelStack, viewStack, projectionStack;
	static float skyBoxScale, skyBoxDistance;

private:
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	//unsigned m_programID;
	//unsigned m_parameters[U_TOTAL];
	/*MS modelStack, viewStack, projectionStack;*/
	double FramesPerSec;

	Camera *camera;

	void RenderSkybox();
	void RenderBaseCamp();

	Light light[8];

	static std::vector<EnvironmentObj*> Env_Obj;
	//std::vector

	bool isPause;
	bool isEscPressed, wasEscPressed;
	

	double deadBossBackToBaseTimer;
	bool fpsonce;
	bool playerDied;
	double countDownbackToBase;
};

#endif