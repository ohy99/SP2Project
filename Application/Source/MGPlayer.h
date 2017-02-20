#ifndef MG_PLAYER_H
#define MG_PLAYER_H

#include "GameObject.h"
#include <vector>
#include "Mesh.h"
#include "Camera.h"

class MGPlayer : public GameObject
{
public:
	void Update(double dt, Camera* camera);
	void MGPlayerMovements(double dt);

	static MGPlayer* getInstance();
	static std::vector<GameObject*> CollisionObject;
	Mesh* getCollisionMesh() { return PMesh[MESH_TYPE::BODY]; } 

private:
	enum MESH_TYPE
	{
		BODY,
		MT_COUNT
	};

	MGPlayer();

	Vector3 pos_;
	Vector3 view_;
	Vector3 right_;

	Mesh* PMesh[MESH_TYPE::MT_COUNT];

	float moveSpeed;
};

#endif