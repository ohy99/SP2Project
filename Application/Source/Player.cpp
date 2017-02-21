#include "Player.h"
#include "Application.h"
#include "Mesh.h"
#include "MainScene.h"
#include "Scene.h"
#include "MeshBuilder.h"
#include "RenderMesh.h"

#include "MeleeWeapon.h"
#include "RangeWeapon.h"

Player* Player::Instance_ = 0;
std::vector<GameObject*> Player::CollisionObjects;
std::vector<EnemyAI*> Player::enemies_;
Player::Player() : GameObject("Player") 
{
	for (size_t i = 0; i < MESH_TYPE::mt_Count; i++)
		PMesh[i] = NULL;
	currentWeapon_ = NULL;
	for (size_t i = 0; i < WEAPON_TYPE::WT_COUNT; i++)
		weapons_[i] = NULL;
	potions = NULL;
	//attack = NULL;
	//int hp_;
	//Vector3 pos_;
	//static Player* Instance_;
	//States state_;

	//Weapon* weapons_[Weapon_types::wt_Count];
	//Weapon* currentWeapon_; 
	PMesh[MESH_TYPE::Body] = MeshBuilder::GenerateOBJ("", "OBJ//goat.obj");
	PMesh[MESH_TYPE::Body]->collisionEnabled = true;
	PMesh[MESH_TYPE::Body]->up = Vector3(0, 1, 0);
	PMesh[MESH_TYPE::Body]->dir = Vector3(0, 0, 1);
	PMesh[MESH_TYPE::Body]->right = Vector3(1, 0, 0);
	dir_.Set(0, 0, 1);

	hp_ = 100;
	hitDelay = 0.0f;


	RangeWeapon* temp_rangedWeap = new RangeWeapon("Ranged", 9, 45, 45, 9);
	temp_rangedWeap->setRangeWeapon(250, 0.8f, 2.f);
	weapons_[WEAPON_TYPE::MELEE] = new MeleeWeapon("Melee", 10);
	weapons_[WEAPON_TYPE::RANGED] = temp_rangedWeap;
	

	currentWeapon_ = weapons_[WEAPON_TYPE::MELEE];
}
Player::~Player()
{
	for (size_t i = 0; i < MESH_TYPE::mt_Count; i++)
	{
		if (PMesh[i] != NULL)
			delete PMesh[i];
	}
	clearCollisionObj();
	if (currentWeapon_)
		delete currentWeapon_;
	for (size_t i = 0; i < WEAPON_TYPE::WT_COUNT; i++)
	{
		if (weapons_[i])
			delete weapons_[i];
	}
	delete potions;
	//delete attack;
	delete Instance_;
}
Player* Player::getInstance() 
{ 
	if (Instance_) 
		return Instance_; 
	else 
		return (Instance_ = new Player()); 
}
void Player::setPosition(Vector3& pos)
{
	pos_ = pos;
}


void Player::update(double dt, Camera* cam)
{
	hitDelay += dt;
	dir_.Set(cam->getDir().x, 0, cam->getDir().z);
	dir_.Normalize();
	right_ = dir_.Normalize().Cross(Vector3(0, 1, 0)).Normalized();
	dirRotateAngle = Math::RadianToDegree(acos(Vector3(0, 0, 1).Dot(Vector3(dir_.x, 0, dir_.z).Normalized())));
	if (dir_ != Vector3(0, 0, 1) && dir_ != Vector3(0, 0, -1))
		dirRotateAxis = Vector3(0, 0, 1).Cross(dir_).Normalized();
	else
		dirRotateAxis.Set(0, 1, 0);
	CollisionMesh_ = PMesh[MESH_TYPE::Body];
//	SceneManager::getCurrentScene()->getCurrentSceneObjs();

	PlayerMovement(dt);
	getPointedObj(cam);

	if (Application::IsKeyPressed('E'))
		currentWeapon_ = weapons_[WEAPON_TYPE::RANGED];
	
	if (currentWeapon_ == weapons_[WEAPON_TYPE::MELEE])
		MeleeAttack(dt);
	else if (currentWeapon_ == weapons_[WEAPON_TYPE::RANGED])
		RangedAttack(dt);

}

void Player::render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	modelStack->PushMatrix();
	modelStack->Translate(pos_.x, pos_.y, pos_.z);
	modelStack->Rotate(dirRotateAngle, dirRotateAxis.x, dirRotateAxis.y, dirRotateAxis.z);
	RenderMeshClass::RenderMesh(PMesh[MESH_TYPE::Body], true, projectionStack, viewStack, modelStack, m_parameters);
	modelStack->PopMatrix();

	if (Pointed_Obj)
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], Pointed_Obj->getName(), Color(0, 1, 0), 2, 35, 55, projectionStack, viewStack, modelStack, m_parameters);

	RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], std::to_string(hp_), Color(0, 1, 0), 2, 0, 55, projectionStack, viewStack, modelStack, m_parameters);

	RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], std::to_string(pos_.x), Color(1, 0, 0), 2, 35, 26, projectionStack, viewStack, modelStack, m_parameters);
	RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], std::to_string(pos_.y), Color(1, 0, 0), 2, 35, 24, projectionStack, viewStack, modelStack, m_parameters);
	RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], std::to_string(pos_.z), Color(1, 0, 0), 2, 35, 22, projectionStack, viewStack, modelStack, m_parameters);

	if (currentWeapon_ == weapons_[WEAPON_TYPE::RANGED])
	{
		RangeWeapon* Rweap = dynamic_cast<RangeWeapon*>(currentWeapon_);
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::SegoeMarker], std::to_string(Rweap->getGunAmmo()), Color(1, 1, 1), 2, 73, 5, projectionStack, viewStack, modelStack, m_parameters);
		//RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::SegoeMarker], std::to_string(Rweap->getWeaponAmmo()), Color(1, 1, 1), 2, 75, 5, projectionStack, viewStack, modelStack, m_parameters);
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::SegoeMarker], std::to_string(Rweap->getWeaponAmmo()), Color(1, 1, 1), 2, 75, 5, projectionStack, viewStack, modelStack, m_parameters);
	}
}


void Player::getPointedObj(Camera* cam)
{
	Position TargetPointFar;
	TargetPointFar.Set(cam->getTarget().x, cam->getTarget().y, cam->getTarget().z);
	Position TargetPointNear;
	TargetPointNear.Set(cam->getPosition().x + (cam->getDir().x * 0.5f),
		cam->getPosition().y + (cam->getDir().y * 0.5f),
		cam->getPosition().z + (cam->getDir().z * 0.5f));

	Pointed_Obj = NULL;

	
	for (auto it : CollisionObjects)
	{
		if (it->CollisionMesh_->isPointInsideAABB(TargetPointNear))
		{
			Pointed_Obj = it; break;
		}
		if (it->CollisionMesh_->isPointInsideAABB(TargetPointFar) == true)
		{
			Pointed_Obj = it;
			break;
		}

	}

}

void Player::isHitUpdate(int dmg)
{
	if (hitDelay >= 1.0f)
	{
		if (this->hp_ > 0)
			this->hp_ -= dmg;
		if (this->hp_ < 0)
			this->hp_ = 0;
		hitDelay = 0.0f;
	}
}
void Player::PlayerMovement(double dt)
{
	//PreCheck collision
	Mesh projected("Projected");
	projected.setHb(true, PMesh[MESH_TYPE::Body]->Hitbox_Min, PMesh[MESH_TYPE::Body]->Hitbox_Max, PMesh[MESH_TYPE::Body]->pos, PMesh[MESH_TYPE::Body]->dir);
	//projected.setHb(true, CollisionMesh_->Hitbox_Min, CollisionMesh_->Hitbox_Max, CollisionMesh_->pos, CollisionMesh_->dir);
	if (Application::IsKeyPressed('W'))
	{
		bool bam = false;
		projected.pos.x += (dir_.x * dt * moveSPD);
		projected.pos.z += (dir_.z * dt * moveSPD);

		for (size_t i = 0; i < CollisionObjects.size(); i++)
		{
			if (projected.isCollide(CollisionObjects[i]->CollisionMesh_) == true)
			{
				bam = true;
				break;
			}
		}

		if (bam == false)
		{
			pos_.x += (dir_.x * dt * moveSPD);
			pos_.z += (dir_.z * dt * moveSPD);
		}
	}
	if (Application::IsKeyPressed('S'))
	{
		bool bam = false;
		projected.pos.x += (-dir_.x * dt * moveSPD);
		projected.pos.z += (-dir_.z * dt * moveSPD);

		for (size_t i = 0; i < CollisionObjects.size(); i++)
		{
			if (projected.isCollide(CollisionObjects[i]->CollisionMesh_) == true)
			{
				bam = true;
				break;
			}
		}

		if (bam == false)
		{
			pos_.x += (-dir_.x * dt * moveSPD);
			pos_.z += (-dir_.z * dt * moveSPD);
		}
	}
	if (Application::IsKeyPressed('A'))
	{
		bool bam = false;
		projected.pos.x += (-right_.x * dt * moveSPD);
		projected.pos.z += (-right_.z * dt * moveSPD);

		for (size_t i = 0; i < CollisionObjects.size(); i++)
		{
			if (projected.isCollide(CollisionObjects[i]->CollisionMesh_) == true)
			{
				bam = true;
				break;
			}
		}

		if (bam == false)
		{
			pos_.x += (-right_.x * dt * moveSPD);
			pos_.z += (-right_.z * dt * moveSPD);
		}
	}
	if (Application::IsKeyPressed('D'))
	{
		bool bam = false;
		projected.pos.x += (right_.x * dt * moveSPD);
		projected.pos.z += (right_.z * dt * moveSPD);

		for (size_t i = 0; i < CollisionObjects.size(); i++)
		{
			if (projected.isCollide(CollisionObjects[i]->CollisionMesh_) == true)
			{
				bam = true;
				break;
			}
		}

		if (bam == false)
		{
			pos_.x += (right_.x * dt * moveSPD);
			pos_.z += (right_.z * dt * moveSPD);
		}
	}
	PMesh[MESH_TYPE::Body]->pos = pos_;
	
}

//bool isDead();
//~Player();

void Player::MeleeAttack(double dt)
{
	static bool hitAlready = false;
	static double hitDelay = 0.0;
	if (Pointed_Obj && glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !hitAlready)
	{
		if (Player::enemies_.size())
		{
			for (auto it : Player::enemies_)
			{
				if (it == Player::getInstance()->getPointedObj())
				{
					it->isHitUpdate(Player::getInstance()->getCurrWeap()->getWeaponDamage());
				}
			}
		}

		hitAlready = true;
	}
	if (hitAlready)
	{
		hitDelay += dt;
		if (hitDelay > 0.5)
		{
			hitDelay = 0.0;
			hitAlready = false;
		}
	}
}

void Player::RangedAttack(double dt)
{
	static bool reloading = false;
	static float reloadTimeElapsed = 0.0f;
	RangeWeapon* Rweap = dynamic_cast<RangeWeapon*>(currentWeapon_);
	if (glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (Rweap->shotCooldown <= 0)
		{
			if (Rweap->Shoot())
			{
				if (Player::enemies_.size())
				{
					for (auto it : Player::enemies_)
					{
						Vector3 NearestHitPt;
						if (it->CollisionMesh_->isLineIntersectAABB(CollisionMesh_->pos, dir_, NearestHitPt))
						{
							it->isHitUpdate(Player::getInstance()->getCurrWeap()->getWeaponDamage());
						}
					}
				}
				Rweap->shotCooldown = 1.0 / ((double)Rweap->roundsPerMin / 60.0);
			}
		}
	}
	else if (Application::IsKeyPressed('R'))
	{
		reloading = true;
	}
	if (reloading)
	{
		reloadTimeElapsed += dt;
		if (reloadTimeElapsed >= Rweap->reloadSpd)
		{
			Rweap->Reload();
			reloading = false;
			reloadTimeElapsed = 0.0f;
		}
	}

	if (Rweap->shotCooldown > 0)
		Rweap->shotCooldown -= dt;
}