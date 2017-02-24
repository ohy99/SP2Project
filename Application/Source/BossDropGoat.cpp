#include "BossDropGoat.h"
#include "BossAI.h"
#include "LoadATOM.h"
#include "RenderMesh.h"
#include "Player.h"

bool BossDGoat::performAttack(double dt, double& animTime)
{
	static const double delayTime = 0.5;
	animTime += dt;

	//GoatMinion* Goat1 = GoatBoss::getInstance()->getInactiveGoatMinion();
	//Goat1->resetGoat();


	//delayTime is for players to react
	if (animTime < delayTime)//Return if havent reach 1s
		return false;

	unsigned performThrice = 3;
	//if (!performOnce)//Spawn projectile once
	while (performThrice && GoatBoss::getInstance()->getInactiveGoatMinion())
	{

		//GoatMinion* newGoat = new GoatMinion;
		//newGoat->CollisionMesh_->pos = Player::getInstance()->CollisionMesh_->pos;
		//GoatBoss::getInstance()->addGoatMinion(newGoat);
		//Player::getInstance()->enemies_.push_back(newGoat);

		GoatMinion* newGoat = GoatBoss::getInstance()->getInactiveGoatMinion();
		newGoat->resetGoat();
		//newGoat->active = false;

		//for (size_t i = 0; i < (sizeof GoatBoss::getInstance()->getMinionPool()) / sizeof(*GoatBoss::getInstance()->getMinionPool()); ++i)
		//{
		//	if (GoatBoss::getInstance()->getMinionPool()[i]->active)
		//		newGoat->CollisionObj.push_back(GoatBoss::getInstance()->getMinionPool()[i]);
		//}
		newGoat->active = true;
		newGoat->CollisionObj.push_back(Player::getInstance());
		float randSX, randSZ;
		randSX = -5.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.f + 5.f)));
		randSZ = -5.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.f + 5.f)));

		newGoat->CollisionMesh_->pos.Set(Player::getInstance()->CollisionMesh_->pos.x + randSX, Player::getInstance()->CollisionMesh_->pos.y, Player::getInstance()->CollisionMesh_->pos.z + randSZ);
		Player::getInstance()->enemies_.push_back(newGoat);
		//Player::getInstance()->addCollisionObject(newGoat);

		//performOnce = true;
		--performThrice;
	}

	performThrice = 0;
	//if (isEndAnim)
	//{
		GoatBoss::getInstance()->resetState();
		return true;
	//	isStartAnim = false;
	//	performOnce = false;
	//}
	//else
	//	DropGoatTime += dt;
	//isStartAnim = true;
	//return true;
}
void BossDGoat::renderAttack(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned* m_parameters)
{
	//if (isStartAnim)
	//{
	//	if (DropGoatTime == 0.0)
	//		isEndAnim = true;
	//}
}