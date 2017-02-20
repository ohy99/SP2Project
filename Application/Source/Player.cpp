#include "Player.h"
#include "Application.h"
#include "Mesh.h"
#include "MainScene.h"
#include "Scene.h"
#include "MeshBuilder.h"
#include "RenderMesh.h"

Player* Player::Instance_ = 0;
std::vector<GameObject*> Player::CollisionObjects;
Player::Player() : GameObject("Player") 
{
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
	dir_.Set(cam->getDir().x, 0, cam->getDir().z);
	dir_.Normalize();
	right_ = dir_.Normalize().Cross(Vector3(0, 1, 0)).Normalized();
	dirRotateAngle = Math::RadianToDegree(acos(Vector3(0, 0, 1).Dot(Vector3(dir_.x, 0, dir_.z).Normalized())));
	if (dir_ != Vector3(0, 0, 1) && dir_ != Vector3(0, 0, -1))
		dirRotateAxis = Vector3(0, 0, 1).Cross(dir_).Normalized();
	else
		dirRotateAxis.Set(0, 1, 0);
	CollisionMesh_ = PMesh[MESH_TYPE::Body];

	PlayerMovement(dt);
	getPointedObj(cam);

}

void Player::render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	modelStack->PushMatrix();
	modelStack->Translate(pos_.x, pos_.y, pos_.z);
	modelStack->Rotate(dirRotateAngle, dirRotateAxis.x, dirRotateAxis.y, dirRotateAxis.z);
	RenderMeshClass::RenderMesh(PMesh[MESH_TYPE::Body], true, projectionStack, viewStack, modelStack, m_parameters);
	modelStack->PopMatrix();

	if (Pointed_Obj)
		RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], Pointed_Obj->getName(), Color(1, 0, 0), 2, 35, 26, projectionStack, viewStack, modelStack, m_parameters);


	RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], std::to_string(pos_.x), Color(1, 0, 0), 2, 35, 26, projectionStack, viewStack, modelStack, m_parameters);
	RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], std::to_string(pos_.y), Color(1, 0, 0), 2, 35, 24, projectionStack, viewStack, modelStack, m_parameters);
	RenderMeshClass::RenderTextOnScreen(&Scene::Text[Scene::TEXT_TYPE::Century], std::to_string(pos_.z), Color(1, 0, 0), 2, 35, 22, projectionStack, viewStack, modelStack, m_parameters);
}


void Player::getPointedObj(Camera* cam)
{
	Position TargetPoint;
	TargetPoint.Set(cam->getTarget().x, cam->getTarget().y, cam->getTarget().z);
	Pointed_Obj = NULL;
}

void Player::isHitUpdate(int dmg)
{
	if (hitDelay >= 1.0f)
	{
		if (this->hp_ > 0)
			this->hp_ - dmg;
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