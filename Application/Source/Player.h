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
#include "EnemyAI.h"
#include "Bullets.h"

#include "Environment.h"

#include "Teleporter.h"

class Player : public GameObject
{
	enum WEAPON_TYPE
	{
		MELEE,
		PISTOL,
		RIFLE,
		MACHINEGUN,
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
	float moveSPD = 20.0f;

	Mesh* Crosshair;

	GameObject* Pointed_Obj;

	Weapon* weapons_[WEAPON_TYPE::WT_COUNT];
	unsigned currWeap = 0;
	Weapon* currentWeapon_;
	Potions* potions; // 
	//AttackType* attack;
	
	//Outfit
	//Inventory
	Mesh* PMesh[MESH_TYPE::mt_Count];
	Player();

	static std::vector<GameObject*> CollisionObjects;
	BulletsTrail* bulletMesh[30];
	BulletsTrail* getBulletTrail();
	void updateBulletTrail(double dt);
	void renderBulletTrail(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);

	bool isFPressed, wasFPressed;


public:
	~Player();
	static std::vector<EnemyAI*> enemies_;
	static Player* getInstance();
	void setPosition(Vector3& pos);
	Vector3 getPlayerPosition() { return pos_; }

	void update(double dt, Camera* cam);
	void render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
	void getPointedObj(Camera* cam);

	void isHitUpdate(int dmg);
	
	Mesh* getCollisionMesh() { return PMesh[MESH_TYPE::Body]; };
	inline int getHp() { return hp_; }
	void PlayerMovement(double dt);
	static inline void addCollisionObject(GameObject* obj) { CollisionObjects.push_back(obj); }

	static void clearCollisionObj(){ while (CollisionObjects.size() > 0) CollisionObjects.pop_back();  
	while (enemies_.size()) enemies_.pop_back(); while (Teleport.size()) Teleport.pop_back();
	}

	GameObject* getPointedObj() { return Pointed_Obj; }
	Weapon* getCurrWeap() { return currentWeapon_; }


	void checkTeleport();
	void checkPickUpItem();
	static std::vector<Teleporter*> Teleport;
	static std::vector<Item*> Items;

	//void TeleportToInsideBarrack();
	//static std::vector<EnvironmentObj*> Teleport_Barrack;

	//bool isDead();
	//~Player();


	void MeleeAttack(double dt);
	void RangedAttack(double dt);

	void checkSwapWeapon();
};

#endif