#include "Camera.h"
#include "Application.h"
#include "Mtx44.h"

/******************************************************************************/
/*!
\brief
Default constructor
*/
/******************************************************************************/
Camera::Camera()
{
}

/******************************************************************************/
/*!
\brief
Destructor
*/
/******************************************************************************/
Camera::~Camera()
{
}

/******************************************************************************/
/*!
\brief
Initialize camera

\param pos - position of camera
\param target - where the camera is looking at
\param up - up vector of camera
*/
/******************************************************************************/
void Camera::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = pos;
	this->target = target;
	this->up = up;
	this->dir = (this->target - this->position).Normalized();
	Bounds.Set(200.0f, 200.0f, 200.0f);
}

/******************************************************************************/
/*!
\brief
Reset the camera settings
*/
/******************************************************************************/
void Camera::Reset()
{
}

/******************************************************************************/
/*!
\brief
To be called every frame. Camera will get user inputs and update its position and orientation

\param dt - frame time
*/
/******************************************************************************/
#include <iostream>
#include <string>
void Camera::Update(double dt)
{
	dt *= 2.0f; //speedz
	this->dir = (this->target - this->position).Normalized();
	Vector3 Right = dir.Cross(this->up).Normalized();
	target = position + dir;


	if (Application::IsKeyPressed('W') && boundCheck(dir,dt))
	//	abs(float(position.x + (dir.x * dt))) <= Bounds.x && abs(float(position.y + (dir.y * dt))) <= Bounds.y && abs(float(position.z + (dir.z * dt))) <= Bounds.z)
	{
		this->position.x += (float)(dir.x * dt);
		this->position.y += (float)(dir.y * dt);
		this->position.z += (float)(dir.z * dt);

		this->target.x += (float)(dir.x * dt);
		this->target.y += (float)(dir.y * dt);
		this->target.z += (float)(dir.z * dt);
	}
	if (Application::IsKeyPressed('S') && boundCheck(-dir, dt))
	//	abs(position.x + (-dir.x * dt)) <= abs(Bounds.x && position.y + (-dir.y * dt)) <= Bounds.y && abs(position.z + (-dir.z * dt)) <= Bounds.z)
	{
		this->position.x += (float)(-dir.x * dt);
		this->position.y += (float)(-dir.y * dt);
		this->position.z += (float)(-dir.z * dt);

		this->target.x += (float)(-dir.x * dt);
		this->target.y += (float)(-dir.y * dt);
		this->target.z += (float)(-dir.z * dt);
	}
	if (Application::IsKeyPressed('A') && boundCheck(-Right, dt))
	{
		this->position.x += (float)(-Right.x * dt);
		this->position.z += (float)(-Right.z * dt);
		this->target.x += (float)(-Right.x * dt);
		this->target.z += (float)(-Right.z * dt);
	}
	if (Application::IsKeyPressed('D') && boundCheck(Right, dt))
	{
		this->position.x += (float)(Right.x * dt);
		this->position.z += (float)(Right.z * dt);
		this->target.x += (float)(Right.x * dt);
		this->target.z += (float)(Right.z * dt);
	}
	
	static const float CAMERA_SPEED = 50.0f;

	if (Application::IsKeyPressed(VK_LSHIFT) && boundCheck(up, dt))
	{
		this->position.y += (float)(up.y * dt);
		this->target.y += (float)(up.y * dt);
	}
	if (Application::IsKeyPressed(VK_LCONTROL) && boundCheck(-up, dt))
	{
		this->position.y += (float)(-up.y * dt);
		this->target.y += (float)(-up.y * dt);
	}

	//system("CLS");
	//std::cout << "Position Vector: " << position.x << ", " << position.y << ", " << position.z << std::endl;
	//std::cout << "target Vector: " << target.x << ", " << target.y << ", " << target.z << std::endl;
	//std::cout << "Dir Vector: " << dir.x << ", " << dir.y << ", " << dir.z << std::endl;
	//std::cout << "Up Vector: " << up.x << ", " << up.y << ", " << up.z << std::endl;
	//Vector3 RightV = (target-position).Cross(up);
	//RightV.Normalize();
	//std::cout << "Right Vector: " << RightV.x << ", " << RightV.y << ", " << RightV.z << std::endl;
	//std::cout << "Cam_Phi: " << Cam_Phi << std::endl;

	
	if (Application::IsKeyPressed(VK_UP))
	{
		if (Cam_Phi < 50.0f)
		{
			float pitch = (float)(CAMERA_SPEED *dt);
			Mtx44 rotation;
			rotation.SetToRotation(pitch, Right.x, Right.y, Right.z);
			dir = rotation * dir;
			up = rotation * up;
			target = position + dir;
			Cam_Phi += (CAMERA_SPEED * dt);
		}
		
	}
	else if (Application::IsKeyPressed(VK_DOWN))
	{
		if (Cam_Phi > -50.0f)
		{
			float pitch = (float)(-CAMERA_SPEED *dt);
			Mtx44 rotation;
			rotation.SetToRotation(pitch, Right.x, Right.y, Right.z);
			dir = rotation * dir;
			up = rotation * up;
			target = position + dir;
			Cam_Phi += (-CAMERA_SPEED * dt);
		}
	}

	else if (Application::IsKeyPressed(VK_LEFT))
	{
			float yaw = (float)(CAMERA_SPEED *dt);
			Mtx44 rotation;
			rotation.SetToRotation(yaw, up.x, up.y, up.z);
			dir = rotation * dir;
			up = rotation * up;
			target = position + dir;
			Cam_Theta += (CAMERA_SPEED * dt);

	}
	else if (Application::IsKeyPressed(VK_RIGHT))
	{
		float yaw = (float)(-CAMERA_SPEED *dt);
			Mtx44 rotation;
			rotation.SetToRotation(yaw, up.x, up.y, up.z);
			dir = rotation * dir;
			up = rotation * up;
			target = position + dir;
			Cam_Theta += (-CAMERA_SPEED * dt);
	}

}
bool Camera::boundCheck(Vector3 path, double dt)
{
	if (abs(float(position.x + (path.x * dt))) <= Bounds.x && abs(float(position.y + (path.y * dt))) <= Bounds.y && abs(float(position.z + (path.z * dt))) <= Bounds.z)
		return true;
	else
		return false;
}
/******************************************************************************/
/*!
\brief
Update Camera based on displacement of Cursor
*/
/**
/******************************************************************************/
void Camera::Update(double dt, double dx, double dy)
{
	dt *= 2.0f; //speedz

	float mouseSpeed = 0.05f;

	float horizontalDISAngle = 0;
	float verticalDISAngle = 0;
	horizontalDISAngle += mouseSpeed * dx; //taking displacement as pi
	verticalDISAngle += mouseSpeed * dy;

	target.Set(
		target.x + cos(verticalDISAngle) * sin(horizontalDISAngle),
		target.y + sin(verticalDISAngle),
		target.z + cos(verticalDISAngle) * cos(horizontalDISAngle)
		);


	this->dir = (this->target - this->position).Normalized();
	Vector3 Right = dir.Cross(this->up).Normalized();
	target = position + dir;

}
