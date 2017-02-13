#ifndef RANGE_WEAPON_H
#define RANGE_WEAPON_H

#include "Weapon.h"

class RangeWeapon : public Weapon
{
public:
	RangeWeapon(const std::string& name, int damage, int totalAmmo, int maxAmmo) : Weapon(name, damage), ammo(maxAmmo), totalAmmo(totalAmmo), maxAmmo(maxAmmo) { ; }
	virtual ~RangeWeapon() { ; }

	// Setter
	void setWeaponAmmo(int ammo);
	void setGunAmmo();

	// Getter
	int getWeaponAmmo();
	int getGunAmmo();

private:
	int ammo, totalAmmo, maxAmmo; // ammo stores the number of ammo in the gun, totalAmmo stores the total number of ammo you have for this particular gun
								  // maxAmmo is just the total number that that particular gun can hold. Eg. Pistol can hold up to 10 Ammo max before reloading,
								  //																	AK47 can hold up to 30 Ammo max before reloading etc..
};

#endif 