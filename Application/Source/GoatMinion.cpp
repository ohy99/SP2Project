#include "GoatMinion.h"
#include "BossAI.h"
#include "Player.h"
#include "RenderMesh.h"
#include "MyMath.h"

GoatMinion::GoatMinion() : EnemyAI("Goat Minion"), doHitDelay(0.5)
{

	//CollisionMesh_ = GoatBoss::goatMinionMesh;
	maxDmg_ = 100;
	minDmg_ = 50;
	doHitCD = 0.0;
	hp_ = 100;
}

GoatMinion::~GoatMinion()
{
	
}
void GoatMinion::update(double dt)
{

	if (!CollisionMesh_->isCollide(Player::getInstance()->CollisionMesh_)) //if bots havent collide with player
	{
		//std::vector<Mesh*> nearObjs;//including trees and car
		doHitCD = 0.0;

		Mesh projected("Projected");//for pre collision check
		projected.setHb(true, CollisionMesh_->Hitbox_Min, CollisionMesh_->Hitbox_Max, CollisionMesh_->pos, CollisionMesh_->dir);



		float Speed = 0.5f;
		Vector3 path;
		path.x = Player::getInstance()->CollisionMesh_->pos.x - (CollisionMesh_->pos).x;
		path.y = Player::getInstance()->CollisionMesh_->pos.y - (CollisionMesh_->pos).y;
		path.z = Player::getInstance()->CollisionMesh_->pos.z - (CollisionMesh_->pos).z;

		projected.pos += (path * (float)dt * Speed);
		

		for (size_t i = 0; i < (sizeof GoatBoss::getInstance()->goatMinionPool) / sizeof(*GoatBoss::getInstance()->goatMinionPool); i++)
		{
			if ((GoatBoss::getInstance()->goatMinionPool)[i] != this && (GoatBoss::getInstance()->goatMinionPool)[i]->active && projected.isCollide((GoatBoss::getInstance()->goatMinionPool)[i]->CollisionMesh_))
				return;
		}
		//for (auto it : CollisionObj)
		//{
		//	if (CollisionMesh_->isCollide(it->CollisionMesh_));
		//	return;
		//}

		try
		{
			CollisionMesh_->dir = path.Normalized();
		}
		catch (DivideByZero what)
		{
			std::cout << "goatminion dir " << what.what() << std::endl;
			CollisionMesh_->dir.Set(0, 0, 1);
		}
		try
		{
			CollisionMesh_->right = CollisionMesh_->dir.Cross(Vector3(0, 1, 0)).Normalized();
		}
		catch (DivideByZero what)
		{
			std::cout << "goatminion right " << what.what() << std::endl;
			CollisionMesh_->dir.Set(1, 0, 0);
		}
		CollisionMesh_->pos.x += (path.x * (float)dt * Speed);
		CollisionMesh_->pos.y += (path.y * (float)dt * Speed);
		CollisionMesh_->pos.z += (path.z * (float)dt * Speed);
	}
	else
	{
		//Player::getInstance()->CollisionMesh_->isHit = true;
		if (doHitCD >= doHitDelay)
		{
			Player::getInstance()->isHitUpdate(getDmg());
			doHitCD = 0.0;
		}
		else
			doHitCD += dt;
	}

	updateShowDmgTaken(dt);
}
void GoatMinion::render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	modelStack->PushMatrix();
	modelStack->Translate(CollisionMesh_->pos.x, CollisionMesh_->pos.y, CollisionMesh_->pos.z);
	modelStack->Rotate((CollisionMesh_->dir.x < 0 ? -1.0f : 1.0f) * Math::RadianToDegree(acos(CollisionMesh_->dir.Dot(Vector3(0, 0, 1)))), 0, 1, 0);
	RenderMeshClass::RenderMesh(CollisionMesh_, true, projectionStack, viewStack, modelStack, m_parameters);
	modelStack->PopMatrix();

	modelStack->PushMatrix();
	modelStack->Translate(0.15f * CollisionMesh_->right.x, 2.f, (0.15f * CollisionMesh_->right.z));
	modelStack->Translate(CollisionMesh_->pos.x, CollisionMesh_->pos.y, CollisionMesh_->pos.z);//y always 0
	modelStack->Rotate((CollisionMesh_->dir.x < 0 ? -1.0f : 1.0f) * Math::RadianToDegree(acos(CollisionMesh_->dir.Dot(Vector3(0.f, 0.f, 1.f)))), 0.f, 1.f, 0.f);
	modelStack->Scale(0.3f, 0.3f, 0.3f);
	RenderMeshClass::RenderText(&Scene::Text[Scene::TEXT_TYPE::Chiller], std::to_string(hp_), Color(1, 0, 0), projectionStack, viewStack, modelStack, m_parameters);

	renderShowDmgTaken(projectionStack, viewStack, modelStack, m_parameters);
	
	modelStack->PopMatrix();
}
void GoatMinion::isHitUpdate(int dmg)
{
	hp_ -= dmg;
	if (hp_ < 0)
		hp_ = 0;

	isHitShowDmgTaken(dmg);
}

int GoatMinion::getDmg()
{
	return (rand() % (maxDmg_ - minDmg_ + 1) + minDmg_);
}
int GoatMinion::getHp()
{
	return hp_;
}
void GoatMinion::resetGoat()
{
	maxDmg_ = 100;
	minDmg_ = 50;
	doHitCD = 0.0;
	hp_ = 100;
	active = true;
	dmgTakenVector.clear();
	for (auto it : CollisionObj)
		it = NULL;
	CollisionObj.clear();
	CollisionMesh_->collisionEnabled = true;
}