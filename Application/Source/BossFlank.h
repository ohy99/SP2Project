#ifndef _BOSSFLANK_H
#define _BOSSFLANK_H

#include "BossAttackStyles.h"
#include "MatrixStack.h"

class BossFlank : public BossAttack
{
	bool performOnce = false;
	bool isStartAnim = false;
	bool isEndAnim = false;
	double FlankTime = 0.0;
public:
	virtual bool performAttack(double dt, double& animTime);
	virtual void renderAttack(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned* m_parameters);
};

#endif