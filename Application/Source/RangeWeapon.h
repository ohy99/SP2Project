#ifndef RANGE_WEAPON_H
#define RANGE_WEAPON_H

#include "Weapon.h"
#include <vector>
#include "Projectile.h"

class RangeWeapon : public Weapon
{
public:
	RangeWeapon(const std::string& name, int damage, unsigned totalAmmo, unsigned maxAmmo, unsigned MagCapacity) :
		Weapon(name, damage), roundsInMag(MagCapacity), ammoNotInMag(totalAmmo), AmmoCapacity(maxAmmo), MagCapacity(MagCapacity) {
		shotCooldown = 0.0;
	}
	virtual ~RangeWeapon() { ; }

	// Setter
	void addAmmo(unsigned ammo);
	void Reload();
	//Set all variables for Range Weap
	void setRangeWeapon(int rpm, float accuracy, float reloadSpd)
	{
		this->roundsPerMin = rpm; this->accuracy = accuracy; this->reloadSpd = reloadSpd;
	}


	// Getter
	int getWeaponAmmo();
	int getGunAmmo();


	bool Shoot();
	double shotCooldown;//This is use to pair with ROF
	int roundsPerMin;
	float accuracy;//Max value is 1.0
	float reloadSpd;

private:
	unsigned int roundsInMag, ammoNotInMag, AmmoCapacity; // ammo stores the number of ammo in the gun, totalAmmo stores the total number of ammo you have for this particular gun
                                  // maxAmmo is just the total number that that particular gun can hold. Eg. Pistol can hold up to 10 Ammo max before reloading,
                                  //																	AK47 can hold up to 30 Ammo max before reloading etc..
	unsigned int MagCapacity;

	
};

#endif 