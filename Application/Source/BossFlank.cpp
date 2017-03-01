#include "BossFlank.h"
#include "BossAI.h"
#include "RenderMesh.h"
#include "LoadATOM.h"

bool BossFlank::performAttack(double dt, double& animTime)
{
	static const double delayTime = 0.5;
	animTime += dt;

	isStartAnim = true;
	FlankTime += dt;

	if (animTime < delayTime)//Return if havent reach 0.5s
		return false;

	static Vector3 randPos;

	if (!performOnce)//Spawn projectile once
	{
		do
		{
			float randSX, randSZ;
			randSX = -15.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (15.f + 15.f)));
			randSZ = -15.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (15.f + 15.f)));

			GoatBoss::getInstance()->CollisionMesh_->pos.Set(GoatBoss::getInstance()->CollisionMesh_->pos.x + GoatBoss::getInstance()->CollisionMesh_->right.x * randSX,
				GoatBoss::getInstance()->CollisionMesh_->pos.y,
				GoatBoss::getInstance()->CollisionMesh_->pos.z + GoatBoss::getInstance()->CollisionMesh_->right.z * randSZ);
		} while (GoatBoss::getInstance()->CollisionMesh_->pos.x >= maxX || GoatBoss::getInstance()->CollisionMesh_->pos.z >= maxZ);
		//redo if the position of the boss is outside of skybox.

		performOnce = true;
	}
	
	
	GoatBoss::getInstance()->resetState();
	return true;
}
void BossFlank::renderAttack(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned* m_parameters)
{
	if (isStartAnim)
	{
		for (size_t i = 0; i < (sizeof GoatBoss::getInstance()->TeleParticles) / (sizeof *GoatBoss::getInstance()->TeleParticles); ++i)
		{
			modelStack->PushMatrix();
			modelStack->Translate(GoatBoss::getInstance()->CollisionMesh_->pos.x, GoatBoss::getInstance()->CollisionMesh_->pos.y, GoatBoss::getInstance()->CollisionMesh_->pos.z);
			LoadAtom("ATOM//BossTeleParticlesAnim.atom", modelStack, &FlankTime, "pCube" + std::to_string(i + 1));
			RenderMeshClass::RenderMesh((GoatBoss::getInstance()->TeleParticles)[i], true, projectionStack, viewStack, modelStack, m_parameters);
			modelStack->PopMatrix();
			if (FlankTime == 0.0)
				isEndAnim = true;
		}
	}
}