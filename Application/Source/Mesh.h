#ifndef MESH_H
#define MESH_H

#include <string>
#include "Material.h"
#include "Vector3.h"
//#include "Vertex.h"
#include "Utility.h"

/******************************************************************************/
/*!
		Class Mesh:
\brief	To store VBO (vertex & color buffer) and IBO (index buffer)
*/
/******************************************************************************/
class Mesh
{


public:
	enum DRAW_MODE
	{
		DRAW_TRIANGLES, //default mode
		DRAW_TRIANGLE_STRIP,
		DRAW_LINES,
		DRAW_MODE_LAST,
	};
	Mesh(const std::string &meshName);
	~Mesh();
	void Render();
	void Render(unsigned offset, unsigned count);

	const std::string name;
	DRAW_MODE mode;
	unsigned vertexBuffer;
	unsigned indexBuffer;
	unsigned indexSize;
	Material material;
	unsigned textureID;

	bool collisionEnabled;
	Vector3 Hitbox_Max;
	Vector3 Hitbox_Min;
	Vector3 pos;
	Vector3 dir;

	Vector3 right;
	Vector3 up;

	void setHb(bool collision, Vector3 min, Vector3 max, Vector3 pos, Vector3 dir) {
		collisionEnabled = collision; Hitbox_Min = min; Hitbox_Max = max; this->pos = pos; this->dir = dir;
	}
	void setHb(bool collision, Vector3 min, Vector3 max, Vector3 pos, Vector3 dir, Vector3 right, Vector3 up) {
		collisionEnabled = collision; Hitbox_Min = min; Hitbox_Max = max; this->pos = pos; this->dir = dir; this->right = right; this->up = up;
	}

	bool isCollide(Mesh* target) {

		return (this->collisionEnabled && target->collisionEnabled &&
			this->pos.x + Hitbox_Max.x >= target->pos.x + target->Hitbox_Min.x &&
			this->pos.x + Hitbox_Min.x <= target->pos.x + target->Hitbox_Max.x &&
			this->pos.y + Hitbox_Max.y >= target->pos.y + target->Hitbox_Min.y &&
			this->pos.y + Hitbox_Min.y <= target->pos.y + target->Hitbox_Max.y &&
			this->pos.z + Hitbox_Max.z >= target->pos.z + target->Hitbox_Min.z &&
			this->pos.z + Hitbox_Min.z <= target->pos.z + target->Hitbox_Max.z
			);
	}

	bool isPointInsideAABB(Position point)
	{

		return (point.x >= this->pos.x + this->Hitbox_Min.x &&
			point.x <= this->pos.x + this->Hitbox_Max.x &&
			point.y >= this->pos.y + this->Hitbox_Min.y &&
			point.y <= this->pos.y + this->Hitbox_Max.y &&
			point.z >= this->pos.z + this->Hitbox_Min.z &&
			point.z <= this->pos.z + this->Hitbox_Max.z
			);
	}

	bool isLineIntersectAABB(Vector3& origin, Vector3& dir, Vector3& outNearPoint)
	{
		Vector3 MinPt = (this->pos + this->Hitbox_Min);
		Vector3 MaxPt = (this->pos + this->Hitbox_Max);
		dir.Normalize();
		Vector3 invDir(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z);

		float lambdaMinX = (MinPt.x - origin.x) * invDir.x;
		float lambdaMaxX = (MaxPt.x - origin.x) * invDir.x;
		float lambdaMinY = (MinPt.y - origin.y) * invDir.y;
		float lambdaMaxY = (MaxPt.y - origin.y) * invDir.y;
		float lambdaMinZ = (MinPt.z - origin.z) * invDir.z;
		float lambdaMaxZ = (MaxPt.z - origin.z) * invDir.z;

		//Rearrange the max and min
		if (lambdaMinX > lambdaMaxX) std::swap(lambdaMinX, lambdaMaxX);
		if (lambdaMinY > lambdaMaxY) std::swap(lambdaMinY, lambdaMaxY);
		if (lambdaMinZ > lambdaMaxZ) std::swap(lambdaMinZ, lambdaMaxZ);

		if ((lambdaMinX > lambdaMaxY) || (lambdaMinY > lambdaMaxX))
			return false;

		if (lambdaMinY > lambdaMinX)
			lambdaMinX = lambdaMinY;

		if (lambdaMaxY < lambdaMaxX)
			lambdaMaxX = lambdaMaxY;

		if ((lambdaMinX > lambdaMaxZ) || (lambdaMinZ > lambdaMaxX))
			return false;
		if (lambdaMinZ > lambdaMinX)
			lambdaMinX = lambdaMinZ;
		if (lambdaMaxZ < lambdaMaxX)
			lambdaMaxX = lambdaMaxZ;

		outNearPoint = origin + (lambdaMinX * dir);
		return true;
	}

};

#endif