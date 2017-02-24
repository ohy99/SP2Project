#ifndef _BULLETS_H
#define _BULLETS_H

#include "Projectile.h"

class BulletsTrail : public Projectile
{

public:
	int dmg;
	bool active;
	bool isHit;
	float tDir;
	float angleRotate;
	Vector3 angleRAxis;
	float pitchAngle;
	float scale;
	Vector3 hitPos;
	Vector3 initialPos;

	BulletsTrail(double spd, int dmg, bool active, bool isHit) : Projectile("Bullet", spd), dmg(dmg), active(active), isHit(isHit), tDir(0.f)
	{
		angleRotate = 0.0f; angleRAxis.Set(0, 1, 0); scale = 0.5f; hitPos.SetZero(); initialPos.SetZero();
	}
	virtual ~BulletsTrail(){}
};

#endif