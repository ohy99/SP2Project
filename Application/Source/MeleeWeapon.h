#ifndef MELEE_WEAPON_H
#define MELEE_WEAPON_H

#include "Weapon.h"

class MeleeWeapon : public Weapon
{
public:
	MeleeWeapon(const std::string& name, int Mindamage, int maxDmg) : Weapon(name, Mindamage, maxDmg) { ; }
	virtual ~MeleeWeapon() { ; }
};

#endif