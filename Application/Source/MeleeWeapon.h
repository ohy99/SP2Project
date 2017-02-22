#ifndef MELEE_WEAPON_H
#define MELEE_WEAPON_H

#include "Weapon.h"

class MeleeWeapon : public Weapon
{
public:
	MeleeWeapon(const std::string& name, int damage) : Weapon(name, damage) { ; }
	virtual ~MeleeWeapon() { ; }
};

#endif