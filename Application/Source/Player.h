#ifndef _PLAYER_H
#define _PLAYER_H

#include "GameObject.h"
#include <vector>
#include "Vector3.h"
#include "Weapon.h"
#include "Potions.h"
#include "Camera.h"
#include "MatrixStack.h"
#include "FPSCam.h"
#include "Environment.h"

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
	Vector3 right_;
	float dirRotateAngle;
	Vector3 dirRotateAxis;
	static Player* Instance_;
	STATES state_;
	float hitDelay = 0.0f;
	float moveSPD = 10.0f;

	GameObject* Pointed_Obj;

	Weapon* weapons_[WEAPON_TYPE::WT_COUNT];
	Weapon* currentWeapon_;
	Potions* potions; // 
	
	//Outfit
	//Inventory
	Mesh* PMesh[MESH_TYPE::mt_Count];
	Player();

	static std::vector<GameObject*> CollisionObjects;
public:
	static Player* getInstance();
	void setPosition(Vector3& pos);

	void update(double dt, Camera* cam);
	void render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
	void getPointedObj(Camera* cam);

	void isHitUpdate(int dmg);

	Mesh* getCollisionMesh() { return PMesh[MESH_TYPE::Body]; };
	inline int getHp() { return hp_; }
	void PlayerMovement(double dt);
	static inline void addCollisionObject(GameObject* obj) { CollisionObjects.push_back(obj); }

	void checkTeleport();
	static std::vector<EnvironmentObj*> Teleport;

	void TeleportToInsideBarrack();
	static std::vector<EnvironmentObj*> Teleport_Barrack;

	//bool isDead();
	//~Player();
};

#endif