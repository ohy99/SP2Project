#ifndef	WEAPON_H
#define WEAPON_H

#include "Item.h"

class Weapon : public Item
{
public: 
	Weapon(const std::string& name, int damage) : Item(name), damage(damage) { ; } // To initialise the name & damage of the weapon
	virtual ~Weapon() {}

	// Getter
	int getWeaponDamage() // To return the damage of the weapon
	{
		return damage;
	}

	// Setter
	void setWeaponDamage(int damage) // When player drinks attack boost potion
	{
		this->damage = this->damage + damage;
	}

protected:
	int damage;
};

#endif