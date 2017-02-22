#ifndef _BOSSATTACKSTYLES_H
#define _BOSSATTACKSTYLES_H

#include "MatrixStack.h"

class BossAttack abstract
{
public:
	BossAttack(){}
	virtual ~BossAttack(){}
	virtual bool performAttack(double dt, double& animTime) = 0;
	virtual void renderAttack(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters) = 0;
};

#endif