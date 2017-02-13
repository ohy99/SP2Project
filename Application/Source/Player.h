#ifndef _PLAYER_H
#define _PLAYER_H

#include "GameObject.h"
#include <vector>
#include "Vector3.h"
#include "Weapon.h"

class Player : public GameObject
{
	enum Weapon_types
	{
		Melee,
		Ranged,
		wt_Count
	};
	enum States
	{
		Idle,
		Walk,
		AimDownSights,
		Shoot,
		States_Count
	};
	int hp_;
	Vector3 pos_;
	static Player* Instance_;
	States state_;

	Weapon* weapons_[Weapon_types::wt_Count];
	Weapon* currentWeapon_;
	Player() : GameObject("Player") {}
public:
	static Player* getInstance() { if (Instance_) return Instance_; else return (Instance_ = new Player()); }
	void setPosition();
};

#endif