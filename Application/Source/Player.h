#ifndef _PLAYER_H
#define _PLAYER_H

#include "GameObject.h"
#include <vector>
#include "Vector3.h"
#include "Weapon.h"
#include "Camera.h"

class Player : public GameObject
{
	enum WEAPON_TYPE
	{
		MELEE,
		RANGED,
		WT_COUNT
	};
	enum STATES
	{
		Idle,
		Walk,
		AimDownSights,
		Shoot,
		States_Count
	};
	enum MESH_TYPE
	{
		Body,
		Arm,
		Head,
		mt_Count
	};
	int hp_;
	Vector3 pos_;
	Vector3 dir_;
	float dirRotateAngle;
	static Player* Instance_;
	STATES state_;
	float hitDelay = 0.0f;

	GameObject* Pointed_Obj;

	Weapon* weapons_[WEAPON_TYPE::WT_COUNT];
	Weapon* currentWeapon_;

	//Outfit
	//Inventory
	Mesh* PMesh[MESH_TYPE::mt_Count];
	Player();
public:
	static Player* getInstance();
	void setPosition(Vector3& pos);

	void update(double dt, Camera* cam);
	void render();
	void getPointedObj(Camera* cam);

	//void isHitUpdate(int dmg);

	Mesh* getCollisionMesh() { return PMesh[MESH_TYPE::Body]; };

	//bool isDead();
	//~Player();
};

#endif