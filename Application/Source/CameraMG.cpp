#include "CameraMG.h"
#include "Application.h"
#include "Mtx44.h"

#include <iostream>
#include <string>
#include "MGPlayer.h"
#include "Player.h"


CameraMG::CameraMG()
{
}

CameraMG::~CameraMG()
{
}

void CameraMG::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = pos;
	//this->target = target;
	this->up = up;
	this->dir = (target - this->position).Normalized();
	Cam_Phi = Math::RadianToDegree(sin(dir.y));
	Cam_Theta = (dir.x < 0 ? 360.0f - Math::RadianToDegree(acos(dir.z)) : Math::RadianToDegree(acos(dir.z)));
	Bounds.Set(10.0f, 10.0f, 10.0f);
}

/******************************************************************************/
/*!
\brief
Reset the CameraMG settings
*/
/******************************************************************************/
void CameraMG::Reset()
{
}


bool CameraMG::boundCheck(Vector3 path, double dt)
{
	if (abs(float(position.x + (path.x * dt))) <= Bounds.x && abs(float(position.y + (path.y * dt))) <= Bounds.y && abs(float(position.z + (path.z * dt))) <= Bounds.z)
		return true;
	else
		return false;
}

#include <GLFW\glfw3.h>
//#include "Application.h"

/******************************************************************************/
/*!
\brief
Update CameraMG based on displacement of Cursor
*/
/**
/******************************************************************************/
void CameraMG::Update(double dt, double dx, double dy)
{
	float mouseSpeed = 0.05f;

	float horizontalDISAngle = 0;
	float verticalDISAngle = 0;
	horizontalDISAngle += mouseSpeed * (float)dx; //taking displacement as pi
	verticalDISAngle += mouseSpeed * (float)dy;
	Cam_Phi += Math::RadianToDegree(verticalDISAngle);

	if (Cam_Theta + Math::RadianToDegree(horizontalDISAngle) > 360.0f)
		Cam_Theta += Math::RadianToDegree(horizontalDISAngle) - 360.0f;

	else if (Cam_Theta + Math::RadianToDegree(horizontalDISAngle) < 0.0f)
		Cam_Theta += 360.0f + Math::RadianToDegree(horizontalDISAngle);

	else
		Cam_Theta += Math::RadianToDegree(horizontalDISAngle);

	dir.Set(
		cos(Math::DegreeToRadian(Cam_Phi)) * sin(Math::DegreeToRadian(Cam_Theta)),
		sin(Math::DegreeToRadian(Cam_Phi)),
		cos(Math::DegreeToRadian(Cam_Phi)) * cos(Math::DegreeToRadian(Cam_Theta))
		);
	dir.Normalize();

	//Vector3 Right = dir.Cross(this->up).Normalized();
	up.Set(
		sin(Math::DegreeToRadian(Cam_Phi)) * -sin(Math::DegreeToRadian(Cam_Theta)),
		cos(Math::DegreeToRadian(Cam_Phi)),
		sin(Math::DegreeToRadian(Cam_Phi)) * -cos(Math::DegreeToRadian(Cam_Theta))
		);
	up.Normalize();

	this->position = MGPlayer::getInstance()->CollisionMesh_->pos;
	this->position.y += 1.8f;

}
