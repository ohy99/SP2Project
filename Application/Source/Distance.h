#ifndef DISTANCE_H
#define DISTANCE_H

#include "Vector3.h"

class Distance
{
public:
	Distance() : objPos(0.f, 0.f, 0.f) { ; }
	~Distance() { ; }

	float checkDistance(Vector3 posOne, Vector3 posTwo) // check distance between Player pos & obj pos. Returns the distance
	{
		float a = posOne.x - posTwo.x;
		float b = posOne.z - posTwo.z;
		sqrt((a*a) + (b*b));

		return sqrt((a*a) + (b*b));
	}

	Vector3 objPos;
};

#endif