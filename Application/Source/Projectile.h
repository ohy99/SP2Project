#ifndef _PROJECTILE_H
#define _PROJECTILE_H

#include "GameObject.h"
#include <string>

class Projectile : public GameObject
{
	double pSpeed_;

public:
	Projectile(const std::string& name, double spd, double deadTime) : GameObject(name), pSpeed_(spd), aliveTime(0.0), deadTime(deadTime) {}
	Projectile(const std::string& name, double spd) : GameObject(name), pSpeed_(spd), aliveTime(0.0), deadTime(0.0) {}
	virtual ~Projectile() {};
	void setSpeed(double spd) { pSpeed_ = spd; }
	double getSpeed() { return pSpeed_; }
	double aliveTime;
	double deadTime;
};

#endif