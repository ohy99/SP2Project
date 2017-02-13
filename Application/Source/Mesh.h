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
	Position Hitbox_Max;
	Position Hitbox_Min;
	Vector3 pos;
	Vector3 dir;
	Position TempMin, TempMax;

	Vector3 right;
	Vector3 up;
	bool isHit = false;

	void setHb(bool collision, Position min, Position max, Vector3 pos, Vector3 dir) {
		collisionEnabled = collision; Hitbox_Min = min; Hitbox_Max = max; this->pos = pos; this->dir = dir; TempMin = Hitbox_Min; TempMax = Hitbox_Max;
	}
	void setHb(bool collision, Position min, Position max, Vector3 pos, Vector3 dir, Vector3 right, Vector3 up) {
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

};

#endif