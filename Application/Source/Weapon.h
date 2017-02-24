#ifndef	WEAPON_H
#define WEAPON_H

#include "Item.h"

class Weapon : public Item
{
public: 
	Weapon(const std::string& name, int Mindmg, int Maxdmg) : Item(name), minDmg(Mindmg), maxDmg(Maxdmg) { ; } // To initialise the name & damage of the weapon
	virtual ~Weapon() { ; }

	// Getter
	inline int getWeaponDamage() // To return the damage of the weapon
	{
		return (rand() % (maxDmg - minDmg + 1) + minDmg);
	}

	// Setter
	void setWeaponDamage(int damage) // When player drinks attack boost potion
	{
		this->minDmg += damage;
		this->maxDmg += damage;
	}

protected:
	int minDmg, maxDmg;
};

#endif