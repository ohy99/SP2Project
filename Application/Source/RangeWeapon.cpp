#include "RangeWeapon.h"

// TODO when player picks up ammo on the floor, chest or whatever, just call this function
void RangeWeapon::setWeaponAmmo(int totalAmmo)
{
	this->totalAmmo = this->totalAmmo + totalAmmo;
}

// TODO when reloading the gun, just call this function
void RangeWeapon::setGunAmmo()
{
	if (totalAmmo >= maxAmmo && ammo < maxAmmo) 
	{
		totalAmmo += ammo;
		ammo -= ammo;
		ammo = maxAmmo;
		totalAmmo -= maxAmmo;
	}
	else if (totalAmmo < maxAmmo && ammo < maxAmmo)
	{
		totalAmmo = totalAmmo + ammo;
		ammo -= ammo;

		if (totalAmmo >= maxAmmo)
		{
			ammo = maxAmmo;
			totalAmmo -= maxAmmo;
		}
		else
		{
			ammo = totalAmmo;
			totalAmmo -= totalAmmo;
		}
	}
}

int RangeWeapon::getWeaponAmmo()
{
	return totalAmmo;
}

int RangeWeapon::getGunAmmo()
{
	return ammo;
}
