#include "Player.h"
#include "Application.h"
#include "Mesh.h"
#include "MainScene.h"
#include "Scene.h"
#include "MeshBuilder.h"
#include "RenderMesh.h"

#include "MeleeWeapon.h"
#include "RangeWeapon.h"
#include "Inventory.h"

#include <iostream>

#define DEFAULTMESHDIR Vector3(0,0,1)

Player* Player::Instance_ = 0;
std::vector<GameObject*> Player::CollisionObjects;

std::vector<EnemyAI*> Player::enemies_;

std::vector<Teleporter*> Player::teleporters_;
std::vector<Item*> Player::Items;
//std::vector<EnvironmentObj*> Player::Teleport_Barrack;

Player::Player() : GameObject("Player"), wasFPressed(false)
{
	for (size_t i = 0; i < MESH_TYPE::mt_Count; i++)
		PMesh[i] = NULL;
	currentWeapon_ = NULL;
	for (size_t i = 0; i < WEAPON_TYPE::WT_COUNT; i++)
		weapons_[i] = NULL;
	Pointed_Obj = NULL;
	potions = NULL;
	bulletHitQuad = NULL;
	PointedAtTeleporter = false;


	//Weapon* weapons_[Weapon_types::wt_Count];
	//Weapon* currentWeapon_; 
	PMesh[MESH_TYPE::Body] = MeshBuilder::GenerateOBJ("", "OBJ//goat.obj");
	PMesh[MESH_TYPE::Body]->collisionEnabled = true;
	PMesh[MESH_TYPE::Body]->up = Vector3(0, 1, 0);
	PMesh[MESH_TYPE::Body]->dir = Vector3(0, 0, 1);
	PMesh[MESH_TYPE::Body]->right = Vector3(1, 0, 0);
	dir_.Set(0, 0, 1);

	hp_ = 1000;
	hitDelay = 0.0f;
	Crosshair = MeshBuilder::GenerateQuad("CrossHair", Color(0, 1, 0), 1, 1);
	Crosshair->textureID = LoadTGA("Image//crosshair.tga");
	for (size_t i = 0; i < (sizeof(bulletMesh) / sizeof(*bulletMesh)); i++)
	{
		bulletMesh[i] = new BulletsTrail(70, 0, false, false);
		bulletMesh[i]->CollisionMesh_ = MeshBuilder::GenerateOBJ("Bullet", "OBJ//BulletTrail.obj");
		bulletMesh[i]->CollisionMesh_->textureID = LoadTGA("Image//BulletTrailUV.tga");
		bulletMesh[i]->CollisionMesh_->material.kShininess = 5.0f;
		bulletMesh[i]->CollisionMesh_->material.kSpecular = 0.8f;
		bulletMesh[i]->CollisionMesh_->collisionEnabled = false;
		bulletMesh[i]->aliveTime = 0.0;
		bulletMesh[i]->deadTime = 1.0;//this time is used to render the bullet hit texture quad
	}
	bulletHitQuad = MeshBuilder::GenerateQuad("bullethit", Color(1, 1, 1), 1.f, 1.f);
	//bulletHitQuad->textureID = LoadTGA("");
	


	RangeWeapon* temp_rangedWeap = new RangeWeapon("PISTOL", 30, 50, 45, 45, 9);
	temp_rangedWeap->setRangeWeapon(250, 0.025f, 2.f);
	weapons_[WEAPON_TYPE::MELEE] = new MeleeWeapon("Melee", 100, 150);
	weapons_[WEAPON_TYPE::PISTOL] = temp_rangedWeap;
	temp_rangedWeap = new RangeWeapon("RIFLE", 55, 75, 90, 120, 30);
	temp_rangedWeap->setRangeWeapon(500, 0.02f, 2.5f);
	weapons_[WEAPON_TYPE::RIFLE] = temp_rangedWeap;
	temp_rangedWeap = new RangeWeapon("MACHINE GUN", 30, 60, 400, 400, 200);
	temp_rangedWeap->setRangeWeapon(800, 0.0275f, 4.f);
	weapons_[WEAPON_TYPE::MACHINEGUN] = temp_rangedWeap;
	temp_rangedWeap = new RangeWeapon("FLAME THROWER? HEH", 10, 25, 1000, 1000, 1000);
	temp_rangedWeap->setRangeWeapon(3000, 0.03f, 3.f);
	weapons_[WEAPON_TYPE::FLAMETHROWER] = temp_rangedWeap;
	temp_rangedWeap = new RangeWeapon("SNIPER", 2000, 3500, 20, 30, 5);
	temp_rangedWeap->setRangeWeapon(100, 0.001f, 2.f);
	weapons_[WEAPON_TYPE::SNIPER] = temp_rangedWeap;

	currentWeapon_ = weapons_[WEAPON_TYPE::MELEE];

	radarDetectRadius = 25;
	//std::vector<Vector3> PositionOfEnemiesInProximity;
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
	delete Crosshair;
	delete[] bulletMesh;
	delete bulletHitQuad;
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
	hitDelay += (float)dt;
	dir_.Set(cam->getDir().x, 0, cam->getDir().z);
	dir_.Normalize();
	right_ = dir_.Normalize().Cross(Vector3(0, 1, 0)).Normalized();
	dirRotateAngle = Math::RadianToDegree(acos(Vector3(0, 0, 1).Dot(Vector3(dir_.x, 0, dir_.z).Normalized())));
	if (dir_ != Vector3(0, 0, 1) && dir_ != Vector3(0, 0, -1))
		dirRotateAxis = Vector3(0, 0, 1).Cross(dir_).Normalized();
	else
		dirRotateAxis.Set(0, 1, 0);
	CollisionMesh_ = PMesh[MESH_TYPE::Body];
	CollisionMesh_->pos = pos_;

	PlayerMovement(dt);
	checkTeleport();
	getPointedObj(cam);

	checkSwapWeapon();

	//TeleportToInsideBarrack();
	//if (currentWeapon_ == weapons_[WEAPON_TYPE::MELEE])
	if (currWeap == WEAPON_TYPE::MELEE)
		MeleeAttack(dt);
	//else if (currentWeapon_ == weapons_[WEAPON_TYPE::PISTOL] || currentWeapon_ == weapons_[WEAPON_TYPE::RIFLE])
	else if (currWeap >= WEAPON_TYPE::PISTOL)
		RangedAttack(dt);


	updateBulletTrail(dt);

	checkPickUpItem();
	checkTeleport();
	updateRadar();
	//TeleportToInsideBarrack();

}

void Player::render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	//modelStack->PushMatrix();
	//modelStack->Translate(pos_.x, pos_.y, pos_.z);
	//modelStack->Rotate(dirRotateAngle, dirRotateAxis.x, dirRotateAxis.y, dirRotateAxis.z);
	//RenderMeshClass::RenderMesh(PMesh[MESH_TYPE::Body], true, projectionStack, viewStack, modelStack, m_parameters);
	//modelStack->PopMatrix();


	renderBulletTrail(projectionStack, viewStack, modelStack, m_parameters);

	RenderMeshClass::RenderMeshOnScreen(Crosshair, (float)Application::getWindowWidth() * 0.5f, (float)Application::getWindowHeight() *0.5f, 5, 50, 50, projectionStack, viewStack, modelStack, m_parameters);
	//Crosshair
	renderRadar(projectionStack, viewStack, modelStack, m_parameters);

	if (PointedAtTeleporter)
	{
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], "[Press SPACE to Teleport into the " + 
			Pointed_Obj->getName() + ".]", Color(0, 1, 0), 2.f, 28, 24,
			projectionStack, viewStack, modelStack, m_parameters);
	}
	else if (Pointed_Obj)
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], Pointed_Obj->getName(), Color(0, 1, 0), 2, 35, 55, projectionStack, viewStack, modelStack, m_parameters);

	RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], std::string("HP:") + std::to_string(hp_), Color(0, 1, 0), 2, 0, 56, projectionStack, viewStack, modelStack, m_parameters);

	RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], std::to_string(pos_.x), Color(0, 1, 0), 1.8, 69, 54, projectionStack, viewStack, modelStack, m_parameters);
	RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], std::to_string(pos_.y), Color(0, 1, 0), 1.8, 69, 52, projectionStack, viewStack, modelStack, m_parameters);
	RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], std::to_string(pos_.z), Color(0, 1, 0), 1.8, 69, 50, projectionStack, viewStack, modelStack, m_parameters);

	if //(currentWeapon_ == weapons_[WEAPON_TYPE::PISTOL] || currentWeapon_ == weapons_[WEAPON_TYPE::RIFLE])
		(currWeap >= WEAPON_TYPE::PISTOL)
	{
		//RangeWeapon* Rweap = dynamic_cast<RangeWeapon*>(currentWeapon_);
		RangeWeapon* Rweap = dynamic_cast<RangeWeapon*>(weapons_[currWeap]);
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::SegoeMarker], std::to_string(Rweap->getGunAmmo()), Color(1, 1, 1), 2, 73, 5, projectionStack, viewStack, modelStack, m_parameters);
		//RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::SegoeMarker], std::to_string(Rweap->getWeaponAmmo()), Color(1, 1, 1), 2, 75, 5, projectionStack, viewStack, modelStack, m_parameters);
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::SegoeMarker], std::to_string(Rweap->getWeaponAmmo()), Color(1, 1, 1), 2, 75, 5, projectionStack, viewStack, modelStack, m_parameters);
	}

}


void Player::getPointedObj(Camera* cam)
{
	Position TargetPointFar;
	TargetPointFar.Set(cam->getPosition().x + (cam->getDir().x * 2.f),
		cam->getPosition().x + (cam->getDir().x * 2.f),
		cam->getPosition().x + (cam->getDir().x * 2.f));
	Position TargetPointNear;
	TargetPointNear.Set(cam->getPosition().x + (cam->getDir().x * 0.5f),
		cam->getPosition().y + (cam->getDir().y * 0.5f),
		cam->getPosition().z + (cam->getDir().z * 0.5f));

	Pointed_Obj = NULL;
	PointedAtTeleporter = false;


	for (auto it : Items)
	{
		if (it->CollisionMesh_->isPointInsideAABB(TargetPointNear))
		{
			Pointed_Obj = it;
			return;
		}
		if (it->CollisionMesh_->isPointInsideAABB(TargetPointFar))
		{
			Pointed_Obj = it;
			return;
		}
	}

	for (auto it : enemies_)
	{
		if (it->CollisionMesh_->isPointInsideAABB(TargetPointNear))
		{
			Pointed_Obj = it; return;
		}
		if (it->CollisionMesh_->isPointInsideAABB(TargetPointFar))
		{
			Pointed_Obj = it; return;
		}
	}
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
	if (hitDelay >= 0.1f)
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

	if (!Inventory::getInstance()->isInventoryOpen() && !UI::getInstance()->isPauseOpen())
	{
		if (Application::IsKeyPressed('W'))
		{
			bool bam = false;
			projected.pos.x += (dir_.x * (float)dt * moveSPD);
			projected.pos.z += (dir_.z * (float)dt * moveSPD);

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
				pos_.x += (dir_.x * (float)dt * moveSPD);
				pos_.z += (dir_.z * (float)dt * moveSPD);
			}
		}
		if (Application::IsKeyPressed('S'))
		{
			bool bam = false;
			projected.pos.x += (-dir_.x * (float)dt * moveSPD);
			projected.pos.z += (-dir_.z * (float)dt * moveSPD);

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
				pos_.x += (-dir_.x * (float)dt * moveSPD);
				pos_.z += (-dir_.z * (float)dt * moveSPD);
			}
		}
		if (Application::IsKeyPressed('A'))
		{
			bool bam = false;
			projected.pos.x += (-right_.x * (float)dt * moveSPD);
			projected.pos.z += (-right_.z * (float)dt * moveSPD);

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
				pos_.x += (-right_.x * (float)dt * moveSPD);
				pos_.z += (-right_.z * (float)dt * moveSPD);
			}
		}
		if (Application::IsKeyPressed('D'))
		{
			bool bam = false;
			projected.pos.x += (right_.x * (float)dt * moveSPD);
			projected.pos.z += (right_.z * (float)dt * moveSPD);

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
				pos_.x += (right_.x * (float)dt * moveSPD);
				pos_.z += (right_.z * (float)dt * moveSPD);
			}
		}

		PMesh[MESH_TYPE::Body]->pos = pos_;
	}	
}

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
	//RangeWeapon* Rweap = dynamic_cast<RangeWeapon*>(currentWeapon_);
	RangeWeapon* Rweap = dynamic_cast<RangeWeapon*>(weapons_[currWeap]);
	if (glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (Rweap->shotCooldown <= 0)
		{
			if (Rweap->Shoot())
			{

				float randSX, randSY, randSZ;
				randSX = -Rweap->accuracy + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (Rweap->accuracy + Rweap->accuracy)));
				randSY = -Rweap->accuracy + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (Rweap->accuracy + Rweap->accuracy)));
				randSZ = -Rweap->accuracy + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (Rweap->accuracy + Rweap->accuracy)));
				float randSX1, randSY1, randSZ1;
				randSX1 = -Rweap->accuracy + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (Rweap->accuracy + Rweap->accuracy)));
				randSY1 = -Rweap->accuracy + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (Rweap->accuracy + Rweap->accuracy)));
				randSZ1 = -Rweap->accuracy + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (Rweap->accuracy + Rweap->accuracy)));

				//bulletOut->CollisionMesh_->dir.Set(bulletOut->CollisionMesh_->dir.x + bulletOut->CollisionMesh_->right.x * randSX + bulletOut->CollisionMesh_->up.x * randSX1,
				//	bulletOut->CollisionMesh_->dir.y + bulletOut->CollisionMesh_->right.y * randSY + bulletOut->CollisionMesh_->up.y * randSY1,
				//	bulletOut->CollisionMesh_->dir.z + bulletOut->CollisionMesh_->right.z * randSZ + bulletOut->CollisionMesh_->up.z * randSZ1);
				//bulletOut->CollisionMesh_->dir.Normalize();

				Vector3 lineDir;
				Vector3 bulletUp, bulletRight, bulletDir;
				bulletRight = right_;
				bulletDir = FPSCam::getInstance()->getDir();
				try{
					bulletUp = bulletRight.Cross(bulletDir).Normalized();
				}
				catch (DivideByZero what){
					bulletUp.Set(0, 1, 0);
				}
				lineDir.Set(bulletDir.x + bulletRight.x * randSX + bulletUp.x * randSX1,
					bulletDir.y + bulletRight.y * randSY + bulletUp.y * randSY1,
					bulletDir.z + bulletRight.z * randSZ + bulletUp.z * randSZ1);
				try{
					lineDir.Normalize();
				}
				catch (DivideByZero){
					lineDir = FPSCam::getInstance()->getDir();
				}

				//ATTEMP to get bullet trail from bulletpool.=========================
				BulletsTrail* bulletOut = getBulletTrail();
				if (bulletOut)
				{
					bulletOut->active = true;
					bulletOut->CollisionMesh_->right = bulletRight;
					bulletOut->CollisionMesh_->dir = lineDir;
					bulletOut->dmg = Rweap->getWeaponDamage();
					bulletOut->CollisionMesh_->pos = FPSCam::getInstance()->getPosition();//bulletmesh initial pos = player pos
					bulletOut->tDir = 0.0f;
					bulletOut->scale = 0.5f;
					bulletOut->initialPos = FPSCam::getInstance()->getPosition();
					bulletOut->aliveTime = 0.0;
					try
					{
						bulletOut->CollisionMesh_->up = bulletOut->CollisionMesh_->dir.Cross(bulletOut->CollisionMesh_->right).Normalized();
					}
					catch (char* what)
					{
						std::cout << what << std::endl;
					}
					bulletOut->CollisionMesh_->dir = lineDir;
				}

			
				//Check if line hit any enemies
				if (Player::enemies_.size())
				{
					for (auto it : Player::enemies_)
					{
						if (bulletOut)
						{
							if (it->CollisionMesh_->isLineIntersectAABB(FPSCam::getInstance()->getPosition(), lineDir, bulletOut->hitPos) && !bulletOut->isHit)
							{
								it->isHitUpdate(bulletOut->dmg);
								bulletOut->isHit = true;
							}
						}
						else//if no active bullet from pool
						{
							if (it->CollisionMesh_->isLineIntersectAABB(FPSCam::getInstance()->getPosition(), lineDir))
							{
								it->isHitUpdate(Rweap->getWeaponDamage());
								break;
							}
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
		reloadTimeElapsed += (float)dt;
		if (reloadTimeElapsed >= Rweap->reloadSpd)
		{
			Rweap->Reload();
			reloading = false;
			reloadTimeElapsed = 0.0f;
		}
	}

	if (Rweap->shotCooldown > 0)
		Rweap->shotCooldown -= (float)dt;
}
void Player::checkTeleport()
{
	if (teleporters_.size() != 0)
	{
		for (auto it : teleporters_)
		{
			if (Pointed_Obj == it)
			{
				PointedAtTeleporter = true;
				if (Application::IsKeyPressed(VK_SPACE))
				{
					SceneManager::getInstance()->SetNextSceneID(it->getNextSceneID());
					SceneManager::getInstance()->SetNextScene();
					break;
				}
			}
		}
	}
}


BulletsTrail* Player::getBulletTrail()
{
	for (size_t i = 0; i < (sizeof(bulletMesh) / sizeof(*bulletMesh)); i++)
	{
		if (!bulletMesh[i]->active)//if false means it is inactive
			return bulletMesh[i];
	}
	return NULL;
}
void Player::updateBulletTrail(double dt)
{
	for (size_t i = 0; i < (sizeof(bulletMesh) / sizeof(*bulletMesh)); i++)
	{
		if (bulletMesh[i]->active)//render active bullets
		{
			bulletMesh[i]->angleRotate = Math::RadianToDegree(acos(DEFAULTMESHDIR.Normalized().Dot(Vector3(bulletMesh[i]->CollisionMesh_->dir.x, 0, bulletMesh[i]->CollisionMesh_->dir.z))));
			bulletMesh[i]->angleRAxis = bulletMesh[i]->CollisionMesh_->up;
			try
			{
				bulletMesh[i]->angleRAxis = DEFAULTMESHDIR.Cross(Vector3(bulletMesh[i]->CollisionMesh_->dir.x, 0, bulletMesh[i]->CollisionMesh_->dir.z).Normalized()).Normalized();
			}
			catch (DivideByZero what)
			{
				std::cout << "Render Bullet Rotate Axis for bullet[" << std::to_string(i) << "]: " << what.what() << std::endl;
			}
			bulletMesh[i]->pitchAngle = Math::RadianToDegree(asin(bulletMesh[i]->CollisionMesh_->dir.y));



			if (!bulletMesh[i]->isHit)
			{
				bulletMesh[i]->scale += (5.f) * (float)dt;
				bulletMesh[i]->CollisionMesh_->pos += bulletMesh[i]->CollisionMesh_->dir * bulletMesh[i]->getSpeed() * (float)dt;
				bulletMesh[i]->tDir += bulletMesh[i]->getSpeed() * (float)dt;
			}
			else
			{
				if ((bulletMesh[i]->hitPos - bulletMesh[i]->CollisionMesh_->pos).Length() >= (bulletMesh[i]->hitPos - bulletMesh[i]->initialPos).Length()) 
					// if pos of bullet to initial position is more than the target to initial pos, means it went over the point. so decrease scaling for more realistic trail
				{
					//if (bulletMesh[i]->scale > 0.5f)
					//	bulletMesh[i]->scale -= (float)(10.0);
					bulletMesh[i]->scale = 0.1f;
					bulletMesh[i]->CollisionMesh_->pos = bulletMesh[i]->hitPos + bulletMesh[i]->CollisionMesh_->dir * bulletMesh[i]->scale;
				}
				bulletMesh[i]->aliveTime += (float)dt;
			}


			if (bulletMesh[i]->tDir > 100)
			{
				bulletMesh[i]->active = false;
				bulletMesh[i]->isHit = false;
			}
			if (bulletMesh[i]->aliveTime >= bulletMesh[i]->deadTime)
			{
				bulletMesh[i]->active = false;
				bulletMesh[i]->isHit = false;
			}
		}
	}
}
void Player::renderBulletTrail(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	
	for (size_t i = 0; i < (sizeof(bulletMesh) / sizeof(*bulletMesh)); i++)
	{
		if (bulletMesh[i]->active)//render active bullets
		{
			modelStack->PushMatrix();
			modelStack->Translate(bulletMesh[i]->CollisionMesh_->pos.x, bulletMesh[i]->CollisionMesh_->pos.y, bulletMesh[i]->CollisionMesh_->pos.z);
			//modelStack->Translate(Player::getInstance()->CollisionMesh_->pos.x, Player::getInstance()->CollisionMesh_->pos.y, Player::getInstance()->CollisionMesh_->pos.z);
			
			modelStack->Rotate(bulletMesh[i]->pitchAngle, bulletMesh[i]->CollisionMesh_->right.x, bulletMesh[i]->CollisionMesh_->right.y, bulletMesh[i]->CollisionMesh_->right.z);
			modelStack->Rotate(bulletMesh[i]->angleRotate, bulletMesh[i]->angleRAxis.x, bulletMesh[i]->angleRAxis.y, bulletMesh[i]->angleRAxis.z);
			modelStack->Scale(1, 1, bulletMesh[i]->scale);
			RenderMeshClass::RenderMesh(bulletMesh[i]->CollisionMesh_, true, projectionStack, viewStack, modelStack, m_parameters);
			modelStack->PopMatrix();
			if (bulletMesh[i]->isHit)
			{
				modelStack->PushMatrix();
				modelStack->Translate(bulletMesh[i]->hitPos.x, bulletMesh[i]->hitPos.y, bulletMesh[i]->hitPos.z);
				modelStack->Rotate(bulletMesh[i]->angleRotate + 180.f, bulletMesh[i]->angleRAxis.x, bulletMesh[i]->angleRAxis.y, bulletMesh[i]->angleRAxis.z);
				modelStack->Scale(0.25f, 0.25f, 1.f);
				RenderMeshClass::RenderMesh(bulletHitQuad, true, projectionStack, viewStack, modelStack, m_parameters);
				modelStack->PopMatrix();
			}
		}
	}
}

void Player::checkSwapWeapon()
{
	static bool wasPressed = false;
	if (Application::IsKeyPressed('E') && !wasPressed)
	{
		currWeap = ++currWeap % WEAPON_TYPE::WT_COUNT;
		//auto it = std::find(weapons_[0], weapons_[(WEAPON_TYPE::WT_COUNT) - 1], currentWeapon_);
		//if (it == (weapons_[(WEAPON_TYPE::WT_COUNT) - 1]))
		//	currentWeapon_ = weapons_[0];
		//else
		//	currentWeapon_ = it++;
		wasPressed = true;
	}
	else if (Application::IsKeyPressed('Q') && !wasPressed)
	{
		if (currWeap)
			currWeap = --currWeap % WEAPON_TYPE::WT_COUNT;
		else
			currWeap = WEAPON_TYPE::WT_COUNT - 1;
		//auto it = std::find(weapons_[0], weapons_[(WEAPON_TYPE::WT_COUNT) - 1], currentWeapon_);
		//if (it == (weapons_[0]))
		//	currentWeapon_ = weapons_[(WEAPON_TYPE::WT_COUNT) - 1];
		//else
		//	currentWeapon_ = it--;
		wasPressed = true;
	}
	if (!Application::IsKeyPressed('E') && !Application::IsKeyPressed('Q') && wasPressed)
		wasPressed = false;

}

void Player::checkPickUpItem()
{
	isFPressed = Application::IsKeyPressed('F');

	if (isFPressed && !wasFPressed)
	{
		for (size_t i = 0; i < Items.size(); i++)
		{
			if (Pointed_Obj && !Inventory::getInstance()->isInventoryFull() && !Items[i]->isItemInInventory)
			{
				Inventory::getInstance()->setItem(Items[i]);
				Items[i]->isItemInInventory = true;
				break;
			}
		}

		wasFPressed = isFPressed;
	}

	if (!isFPressed && wasFPressed)
		wasFPressed = isFPressed;

}

GameObject* Player::removeCollisionObject(GameObject* obj) {
	auto it = std::find(Player::getInstance()->CollisionObjects.begin(), Player::getInstance()->CollisionObjects.end(), obj);
	if (!*it)
		return NULL;
	if (it != Player::getInstance()->CollisionObjects.end())
		std::swap(*it, Player::getInstance()->CollisionObjects.back());
	//goatMinionPool[i]->CollisionMesh_->pos = Vector3(0, -5, 0);
	GameObject* ret = *it;
	Player::getInstance()->CollisionObjects.back() = NULL;
	Player::getInstance()->CollisionObjects.pop_back();
	return ret;
}

void Player::updateRadar()
{
	PositionOfEnemiesInProximity.clear();
	for (auto it : enemies_)
	{
		if ((it->CollisionMesh_->pos - Player::CollisionMesh_->pos).LengthSquared() < radarDetectRadius * radarDetectRadius)//not using sqrt would be faster
		{
			PositionOfEnemiesInProximity.push_back(it->CollisionMesh_->pos - Player::CollisionMesh_->pos);
		}
	}
}
void Player::renderRadar(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	//This is the player location
	RenderMeshClass::RenderMeshOnScreen(bulletHitQuad, (float)Application::getWindowWidth() * 0.5f, (float)Application::getWindowHeight() * 0.5f
		, 1, 0.01f *(float)Application::getWindowWidth(), 0.01f * (float)Application::getWindowHeight(), projectionStack, viewStack, modelStack, m_parameters);
	for (auto it: PositionOfEnemiesInProximity)
	{
		modelStack->PushMatrix();
		//modelStack->Translate();
		RenderMeshClass::RenderMeshOnScreen(bulletHitQuad, (float)Application::getWindowWidth() * 0.5f + it.x, (float)Application::getWindowHeight() * 0.5f + it.z
			, 1.5f, 0.01f *(float)Application::getWindowWidth(), 0.01f * (float)Application::getWindowHeight(), projectionStack, viewStack, modelStack, m_parameters);
		modelStack->PopMatrix();
	}
}