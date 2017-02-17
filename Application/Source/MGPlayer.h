#ifndef MG_PLAYER_H
#define MG_PLAYER_H

#include "GameObject.h"
#include "Mesh.h"

class MGPlayer : public GameObject
{
public:
	void Update(double dt);


private:
	enum MESH_TYPE
	{
		BODY,
		MT_COUNT
	};

	MGPlayer();

	Vector3 dir_;
	Vector3 right_;

	Mesh* PMesh[MESH_TYPE::MT_COUNT];

	float moveSpeed;
};

#endif