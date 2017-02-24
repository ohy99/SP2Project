#ifndef _BOSSDROPGOAT_H
#define _BOSSDROPGOAT_H

#include "BossAttackStyles.h"
#include "MatrixStack.h"

class BossDGoat : public BossAttack
{
	bool performOnce = false;
	bool isStartAnim = false;
	bool isEndAnim = false;
	double DropGoatTime = 0.0;
public:
	virtual bool performAttack(double dt, double& animTime);
	virtual void renderAttack(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned* m_parameters);
};

#endif