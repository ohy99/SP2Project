#ifndef CAMERA_MG_H
#define CAMERA_MG_H

#include "Vector3.h"
#include "Mtx44.h"
#include "Vertex.h"

#include "Camera.h"

class CameraMG : public Camera
{

	Vector3 position;
	//	Vector3 target;
	Vector3 dir;
	Vector3 up;
	Vector3 Right;

	float Cam_Phi = 0.0f;
	float Cam_Theta = 180.0f;

	float maxVerticalAngle, maxHorizontalAngle;

	bool boundCheck(Vector3 path, double dt);

	Position Bounds;
public:

	CameraMG();
	~CameraMG();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Reset();
	//virtual void Update(double dt)
	virtual void Update(double dt, double dx, double dy);
	virtual Vector3 getPosition() { return position; };
	virtual Vector3 getTarget() { return position + dir; };
	virtual Vector3 getDir() { return dir; };
	virtual Vector3 getUp() { return up; };
	virtual Vector3 getRight() { return Right; };

	//protected:
};

#endif