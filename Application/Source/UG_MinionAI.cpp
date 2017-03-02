#include "UG_MinionAI.h"
#include "UndergroundScene.h"
#include "Player.h"
#include "RenderMesh.h"
#include "MyMath.h"
#include "MeshBuilder.h"

//UG_MinionAI* UndergroundScene::goatMinionPool[5];

UG_MinionAI::UG_MinionAI() : EnemyAI("Goat Minion"), doHitDelay(0.5)
{

	CollisionMesh_ = MeshBuilder::GenerateOBJ("", "OBJ//goat.obj");
	CollisionMesh_->collisionEnabled = true;

	resetMinion();

	//maxDmg_ = 100;
	//minDmg_ = 50;
	//doHitCD = 0.0;
	//hp_ = 100;
}

UG_MinionAI::~UG_MinionAI()
{

}

void UG_MinionAI::update(double dt)
{

	if (!CollisionMesh_->isCollide(Player::getInstance()->CollisionMesh_)) //if bots havent collide with player
	{
		doHitCD = 0.0;

		//IF DISTANCE FROM MINION TO PLAYER IS LESS THAN 5 UNITS AWAY  OR  Minion is hit by player
		if ((Player::getInstance()->CollisionMesh_->pos - this->CollisionMesh_->pos).Length() < 10 || this->chasePlayer)
		{
			runToPlayer(dt);
			chasePlayer = true;
		}
	}
	else
	{
		//Player::getInstance()->CollisionMesh_->isHit = true;
		if (doHitCD >= doHitDelay * attSpdDelayMultiplier)
		{
			Player::getInstance()->isHitUpdate(getDmg());
			doHitCD = 0.0;
		}
		else
			doHitCD += dt;
	}
}

void UG_MinionAI::render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
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
	modelStack->PopMatrix();
}

void UG_MinionAI::isHitUpdate(int dmg)
{
	hp_ -= dmg;
	if (hp_ < 0)
		hp_ = 0;
	//isHit = true;
	chasePlayer = true;
}

int UG_MinionAI::getDmg()
{
	return (rand() % (maxDmg_ - minDmg_ + 1) + minDmg_) * dmgMultiplier;
}

int UG_MinionAI::getHp()
{
	return hp_;
}

void UG_MinionAI::resetMinion()
{
	maxDmg_ = 100;
	minDmg_ = 50;
	doHitCD = 0.0;
	hp_ = 200;
	chasePlayer = false;
	//isHit = false;
	deadTime = 0.0;

	active = true;

	CollisionMesh_->collisionEnabled = true;

	//30x, 18z //103x, -96z
	if (rand() % 2)
	{
		bool kenaStuck = true;
		while (kenaStuck)
		{
			kenaStuck = false;
			CollisionMesh_->pos.x = -33.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-28.f + 38.f)));
			CollisionMesh_->pos.z = -32.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-27.f + 37.f)));
			for (auto it : UndergroundScene::Env_Obj)
			{
				if (it->CollisionMesh_->isCollide(this->CollisionMesh_))
				{
					kenaStuck = true;
					break;
				}
			}
			for (size_t i = 0; i < (sizeof UndergroundScene::UG_EnemyPool) / sizeof(*UndergroundScene::UG_EnemyPool); i++)
			{
				if (UndergroundScene::UG_EnemyPool[i])
				{
					if (UndergroundScene::UG_EnemyPool[i] != this &&
						UndergroundScene::UG_EnemyPool[i]->active &&
						this->CollisionMesh_->isCollide(UndergroundScene::UG_EnemyPool[i]->CollisionMesh_))//Because this function is called during init, not all enemy classes are created yet, so theere will
						//be null pointers which will cause programe to crash when trying to access NuLL
					{
						kenaStuck = true;
						break;
					}
				}
			}
		}
	}
	else
	{
		bool kenaStuck = true;
		while (kenaStuck)
		{
			kenaStuck = false;
			CollisionMesh_->pos.x = 32.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (37.f - 27.f)));
			CollisionMesh_->pos.z = -32.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-27.f + 37.f)));
			for (auto it : UndergroundScene::Env_Obj)
			{
				if (it->CollisionMesh_->isCollide(this->CollisionMesh_))
				{
					kenaStuck = true;
					break;
				}
			}
			for (size_t i = 0; i < (sizeof UndergroundScene::UG_EnemyPool) / sizeof(*UndergroundScene::UG_EnemyPool); i++)
			{
				if (UndergroundScene::UG_EnemyPool[i])
				{
					if (UndergroundScene::UG_EnemyPool[i] != this &&
						UndergroundScene::UG_EnemyPool[i]->active &&
						this->CollisionMesh_->isCollide(UndergroundScene::UG_EnemyPool[i]->CollisionMesh_))//Because this function is called during init, not all enemy classes are created yet, so theere will
						//be null pointers which will cause programe to crash when trying to access NuLL
					{
						kenaStuck = true;
						break;
					}
				}
			}
		}

	}

	Player::getInstance()->enemies_.push_back(this);
	Player::getInstance()->addCollisionObject(this);
}

void UG_MinionAI::runToPlayer(double dt)
{
	Mesh projected("Projected");//for pre collision check
	projected.setHb(true, CollisionMesh_->Hitbox_Min, CollisionMesh_->Hitbox_Max, CollisionMesh_->pos, CollisionMesh_->dir);



	//float Speed = 0.5f;
	Vector3 path;
	path.x = Player::getInstance()->CollisionMesh_->pos.x - (CollisionMesh_->pos).x;
	path.y = Player::getInstance()->CollisionMesh_->pos.y - (CollisionMesh_->pos).y;
	path.z = Player::getInstance()->CollisionMesh_->pos.z - (CollisionMesh_->pos).z;

	projected.pos += (path * (float)dt * moveSpd);

	for (size_t i = 0; i < (sizeof UndergroundScene::UG_EnemyPool) / sizeof(*UndergroundScene::UG_EnemyPool); i++)
	{
		if (UndergroundScene::UG_EnemyPool[i] != this &&
			UndergroundScene::UG_EnemyPool[i]->active &&
			projected.isCollide(UndergroundScene::UG_EnemyPool[i]->CollisionMesh_))
			return;
	}
	for (auto it : UndergroundScene::Env_Obj)
	{
		if (it->CollisionMesh_->isCollide(&projected))
			return;
	}

	try
	{
		CollisionMesh_->dir = path.Normalized();
	}
	catch (DivideByZero what)
	{
		std::cout << "UG_MinionAI dir " << what.what() << std::endl;
		CollisionMesh_->dir.Set(0, 0, 1);
	}
	try
	{
		CollisionMesh_->right = CollisionMesh_->dir.Cross(Vector3(0, 1, 0)).Normalized();
	}
	catch (DivideByZero what)
	{
		std::cout << "UG_MinionAI right " << what.what() << std::endl;
		CollisionMesh_->dir.Set(1, 0, 0);
	}
	CollisionMesh_->pos.x += (path.x * (float)dt * moveSpd);
	CollisionMesh_->pos.y += (path.y * (float)dt * moveSpd);
	CollisionMesh_->pos.z += (path.z * (float)dt * moveSpd);
}