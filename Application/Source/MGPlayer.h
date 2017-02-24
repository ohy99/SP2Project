#ifndef MG_PLAYER_H
#define MG_PLAYER_H

#include "GameObject.h"
#include <vector>
#include "MatrixStack.h"
#include "Mesh.h"
#include "Camera.h"
#include "Distance.h"

class MGPlayer : public GameObject
{
public:
	void Update(double dt, Camera* camera);
	void Render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
	void MGPlayerMovements(double dt);

	static MGPlayer* getInstance();
	void setPlayerPosition(Vector3& pos);
	Vector3 getPlayerPosition() { return pos_; }

	float playerScore(float scoreMultiplier);

	static std::vector<GameObject*> CollisionObjects;
	Mesh* getCollisionMesh() { return PMesh[MESH_TYPE::BODY]; } 
	static inline void addCollisionObject(GameObject* obj) { CollisionObjects.push_back(obj); }

private:
	enum MESH_TYPE
	{
		BODY,
		MT_COUNT
	};

	MGPlayer();
	~MGPlayer();

	Vector3 pos_;
	Vector3 dir_;
	Vector3 right_;

	Mesh* PMesh[MESH_TYPE::MT_COUNT];

	static MGPlayer* Instance;

	float lane;
	float moveSpeed;

	bool isHit;
	bool isDead();
	bool isDPressed, wasDPressed, isAPressed, wasAPressed;

	Distance distance;
};

#endif