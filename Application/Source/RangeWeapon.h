#ifndef RANGE_WEAPON_H
#define RANGE_WEAPON_H

#include "Weapon.h"

class RangeWeapon : public Weapon
{
public:
	RangeWeapon(const std::string& name, int damage) : Weapon(name, damage) { ; }
	virtual ~RangeWeapon() { ; }

	// Setter
	void setWeaponAmmo(int ammo);
	void setGunAmmo();

	// Getter
	int getWeaponAmmo();
	int getGunAmmo();

private:
	int ammo, totalAmmo; // ammo stores the number of ammo in the gun, totalAmmo stores the total number of ammo you have for this particular gun
};

#endif 