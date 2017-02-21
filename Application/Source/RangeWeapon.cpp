#include "RangeWeapon.h"

// TODO when player picks up roundsInMag on the floor, chest or whatever, just call this function
void RangeWeapon::addAmmo(unsigned totalAmmo)
{
	this->ammoNotInMag = this->ammoNotInMag + totalAmmo;
	if (this->ammoNotInMag > this->AmmoCapacity)
		totalAmmo = AmmoCapacity;
}

// TODO when reloading the gun, just call this function
void RangeWeapon::Reload()
{
	if (ammoNotInMag)//reload possible if there are rounds not inside magazine
	{
		ammoNotInMag += roundsInMag;//Add rounds in Mag to rounds not inside mag
		if (ammoNotInMag > MagCapacity)//if there are more ammo than a mag can hold
		{
			roundsInMag = MagCapacity;
			ammoNotInMag -= MagCapacity;//attempt to add reload bullets to mag cap
		}
		else//if there are less ammo than a mag can hold
		{
			roundsInMag = ammoNotInMag;
			ammoNotInMag = 0;
		}
	}

	//if (ammoNotInMag >= AmmoCapacity && roundsInMag < AmmoCapacity)
	//{
	//	ammoNotInMag += roundsInMag;
	//	roundsInMag -= roundsInMag;
	//	roundsInMag = AmmoCapacity;
	//	ammoNotInMag -= AmmoCapacity;
	//}
	//else if (ammoNotInMag < AmmoCapacity && roundsInMag < AmmoCapacity)
	//{
	//	ammoNotInMag = ammoNotInMag + roundsInMag;
	//	roundsInMag -= roundsInMag;

	//	if (ammoNotInMag >= AmmoCapacity)
	//	{
	//		roundsInMag = AmmoCapacity;
	//		ammoNotInMag -= AmmoCapacity;
	//	}
	//	else
	//	{
	//		roundsInMag = ammoNotInMag;
	//		ammoNotInMag -= ammoNotInMag;
	//	}
	//}
}

int RangeWeapon::getWeaponAmmo()
{
	return ammoNotInMag;
}

int RangeWeapon::getGunAmmo()
{
	return roundsInMag;
}

bool RangeWeapon::Shoot()
{
	if (roundsInMag)
	{
		roundsInMag--;
		return true;
	}
	return false;
}