#include "BossShootProj.h"
#include "Projectile.h"
#include "MeshBuilder.h"
#include "BossAI.h"
#include "Player.h"

bool BossShootProj::performAttack(double dt, double& animTime)
{
	static const double delayTime = 1.0;
	animTime += dt;

	//delayTime is for players to react
	if (animTime < delayTime)//Return if havent reach 1s
		return false;

	if (!performOnce)//Spawn projectile once
	{
		Projectile* temp_proj;
		temp_proj = GoatBoss::getInstance()->projMesh;
		temp_proj->CollisionMesh_->pos = GoatBoss::getInstance()->CollisionMesh_->pos;
		temp_proj->CollisionMesh_->dir = GoatBoss::getInstance()->dirBossToPlayer;

		GoatBoss::getInstance()->addProjectiles(temp_proj);
		performOnce = true;
	}

	
	//if (isEndAOE)
	//{
		GoatBoss::getInstance()->resetState();
		return true;
	//	isStartAOE = false;
	//	performOnce = false;
	//}
	//else
	//	ShootAnimTime += dt;
	//isStartAOE = true;
	return true;
}
void BossShootProj::renderAttack(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	//if (isStartAOE)
	//{

	//	if (ShootAnimTime == 0.0)
	//		isEndAOE = true;
	//}
}