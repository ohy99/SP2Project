#ifndef _BOSSGSMASH_H
#define _BOSSGSMASH_H

#include "BossAttackStyles.h"

class BossGSmash : public BossAttack
{
	bool performOnce = false;
	bool isStartAOE = false;
	bool isEndAOE = false;
	double SmashAOETime = 0.0;
public:
	virtual bool performAttack(double dt, double& animTime);
	virtual void renderAttack(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
};

#endif