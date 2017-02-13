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
		Melee,
		Ranged,
		wt_Count
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
	static Player* Instance_;
	STATES state_;
	
	GameObject* Pointed_Obj;

	Weapon* weapons_[WEAPON_TYPE::wt_Count];
	Weapon* currentWeapon_;
	Mesh* PMesh[MESH_TYPE::mt_Count];
	Player();
public:
	static Player* getInstance();
	void setPosition(Vector3& pos);

	void update(double dt, Camera* cam);
	void render();
	void getPointedObj(Camera* cam);
};

#endif