#ifndef POTIONS_H
#define POTIONS_H

#include "Usables.h"

// This class is to give the player buffs so that they have a slight adventage over the enemy
// can make item such that when use, it increases health & attack for a certain period

class Potions : public Usables
{
public:
	Potions(std::string& name, signed int count, int health, int attackBoost, int speedBoost) : Usables(name, count), health(health), attackBoost(attackBoost), speedBoost(speedBoost) { ; }
	virtual ~Potions() { ; }

	// Getters 
	// Use these functions in player class accordingly
	int getHealth() { return health; }
	int getAttackBoost() { return attackBoost; }
	int getSpeedBoost() { return speedBoost; }

private:
	int health, attackBoost, speedBoost; // health potion increases maxHP and/or recover a certain amount of HP
};

#endif