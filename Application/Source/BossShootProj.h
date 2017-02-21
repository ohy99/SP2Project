#ifndef _BOSSSHOOTPROJECTILE_H
#define _BOSSSHOOTPROJECTILE_H

#include "BossAttackStyles.h"
#include "Projectile.h"

class BossShootProj : public BossAttack
{
	bool performOnce = false;
	bool isStartAOE = false;
	bool isEndAOE = false;
	double ShootAnimTime = 0.0;
public:
	bool performAttack(double dt, double& animTime);
	virtual void renderAttack(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
};

#endif