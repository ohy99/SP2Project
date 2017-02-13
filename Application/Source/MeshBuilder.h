#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "Vertex.h"

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);
	static Mesh* GenerateQuad(const std::string &meshName, Color color, float lengthX, float lengthY);
	static Mesh* GenerateCube(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ);
	static Mesh* GenerateRing(const std::string &meshName, Color color, unsigned int numSlices = 10, float radius = 1, float innerradius = 0);
	static Mesh* GenerateSphere(const std::string &meshName, Color color, unsigned int numStacks = 10, unsigned int numSlices = 10, float radius = 1);
	static Mesh* GenerateHemisphere(const std::string &meshName, Color color, unsigned int numStacks = 10, unsigned int numSlices = 10, float radius = 1);
	static Mesh* GenerateTorus(const std::string &meshName, Color color, unsigned int numStacks = 10, unsigned int numSlices = 10, float innerRadius = 5, float outerRadius = 1);
	static Mesh* GenerateCylinder(const std::string &meshName, Color color, unsigned int numSlices = 10, float BaseRadius = 1, float TopRadius = 1, float height = 5);
	static Mesh* GenerateOBJ(const std::string &meshName, const std::string &file_path);
	static Mesh* GenerateText(const std::string &meshName, unsigned numRow, unsigned numCol);
};


#endif