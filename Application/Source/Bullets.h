#ifndef _BULLETS_H
#define _BULLETS_H

#include "Projectile.h"

class Bullets : public Projectile
{
	int dmg;
	bool active;
	bool isHit;
};

#endif