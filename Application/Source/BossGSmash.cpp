#include "BossGSmash.h"
#include "BossAI.h"
#include "Player.h"
#include "RenderMesh.h"
#include "LoadATOM.h"

bool BossGSmash::performAttack(double dt, double& animTime)
{
	static const double GSDelayTime = 1.0;
	animTime += dt;
	if (animTime < GSDelayTime)
		return false;

	if (!performOnce)
	{
		if ((Player::getInstance()->CollisionMesh_->pos - GoatBoss::getInstance()->CollisionMesh_->pos).Length() < 5)
		{
			Player::getInstance()->isHitUpdate(10);
			//knockback

			performOnce = true;
		}
	}

	//Need time for reset state.
	if (isEndAOE)
	{
		GoatBoss::getInstance()->resetState();
		return true;
		isStartAOE = false;
		performOnce = false;
	}
	else
		SmashAOETime += dt;
	isStartAOE = true;
	return true;
}
void BossGSmash::renderAttack(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	if (isStartAOE)
	{
		modelStack->PushMatrix();
		//AOESmash
		LoadAtom("ATOM//GSAnim.atom", modelStack, &SmashAOETime, "pSphere1");
		RenderMeshClass::RenderMesh(GoatBoss::AOESmash, true, projectionStack, viewStack, modelStack, m_parameters);
		modelStack->PopMatrix();

		if (SmashAOETime == 0.0)
			isEndAOE = true;
	}
}