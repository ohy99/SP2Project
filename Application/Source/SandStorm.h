#ifndef _SANDSTORM_H
#define _SANDSTORM_H

#include "Mesh.h"
#include "MatrixStack.h"
#include <vector>


class SandStorm
{
	struct DustParticles
	{
		enum QUAD
		{
			QUADRANT1,
			QUADRANT2,
			QUADRANT3,
			QUADRANT4,
			pXAXIS,
			nXAXIS,
			pZAXIS,
			nZAXIS,
		};
		float scaleX, scaleY;
		Mesh* dustQuad;
		float rotationUp;
		float speed;
		void resetDust();
	};
	static const float outerBorderX;
	static const float outerBorderZ;
	static DustParticles::QUAD spawnAtQuad;
	static Vector3* dustDir;
	float rotateAngleDir;
	Vector3 rotateAxis;
	void checkQuadrants(Vector3);
	std::vector<DustParticles*> dust_;
	static SandStorm* instance_;
	SandStorm();
public:
	~SandStorm();
	static SandStorm* getInstance();
	void update(double dt);
	void render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
};


#endif