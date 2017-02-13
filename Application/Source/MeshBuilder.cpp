#include "MeshBuilder.h"
#include <GL\glew.h>
#include <vector>
#include "Vertex.h"
#include "MyMath.h"
#include "LoadOBJ.h"

Mesh* MeshBuilder::GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ)
{
	Vertex v;
	std::vector<Vertex> VBO;
	std::vector<unsigned> index_buffer_data;

	v.pos.Set(-lengthX/2, 0, 0); v.color.Set(1, 0, 0); VBO.push_back(v);
	v.pos.Set(lengthX / 2, 0, 0); v.color.Set(1, 0, 0); VBO.push_back(v);

	v.pos.Set(0, -lengthY / 2, 0); v.color.Set(0, 1, 0); VBO.push_back(v);
	v.pos.Set(0, lengthY / 2, 0); v.color.Set(0, 1, 0); VBO.push_back(v);

	v.pos.Set(0, 0, -lengthZ / 2); v.color.Set(0, 0, 1); VBO.push_back(v);
	v.pos.Set(0, 0, lengthZ / 2); v.color.Set(0, 0, 1); VBO.push_back(v);

	for (unsigned i = 0; i < 6; ++i)
		index_buffer_data.push_back(i);

	Mesh *mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, VBO.size() * sizeof(Vertex), &VBO[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(unsigned), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_LINES;

	return mesh;
}

Mesh* MeshBuilder::GenerateQuad(const std::string &meshName, Color(color), float lengthX, float lengthY)
{
	Vertex v;

	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;

	float lengthZ = 0.0f;

		//-lengthX/2 to lengthX/2
		//front

	v.pos.Set(-lengthX / 2.0f, lengthY / 2.0f, lengthZ / 2.0f); v.color = color; v.normal.Set(0,0,1); v.texCoord.Set(0, 1); vertex_buffer_data.push_back(v);
		v.pos.Set(-lengthX/2.0f, -lengthY/2.0f, lengthZ/2.0f); v.color = color; v.normal.Set(0,0,1); v.texCoord.Set(0, 0);vertex_buffer_data.push_back(v);
		v.pos.Set(lengthX/2.0f, lengthY/2.0f, lengthZ/2.0f); v.color = color; v.normal.Set(0,0,1); v.texCoord.Set(1, 1);vertex_buffer_data.push_back(v);

		v.pos.Set(lengthX/2.0f, lengthY/2.0f, lengthZ/2.0f); v.color = color; v.normal.Set(0,0,1); v.texCoord.Set(1, 1);vertex_buffer_data.push_back(v);
		v.pos.Set(-lengthX/2.0f, -lengthY/2.0f, lengthZ/2.0f); v.color = color; v.normal.Set(0,0,1); v.texCoord.Set(0, 0);vertex_buffer_data.push_back(v);
		v.pos.Set(lengthX/2.0f, -lengthY/2.0f, lengthZ/2.0f); v.color = color; v.normal.Set(0,0,1); v.texCoord.Set(1, 0);vertex_buffer_data.push_back(v);


	for (unsigned i = 0; i < vertex_buffer_data.size(); ++i)
	{
		index_buffer_data.push_back(i);
	}

	//v.texCoord.Set(0, 0);
	//v.texCoord.Set(1, 0);
	//v.texCoord.Set(1, 1);
	//v.texCoord.Set(0, 1);


	Mesh *mesh = new Mesh(meshName);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer); //bind index buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(unsigned), &index_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);



	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;


	return mesh;
}


Mesh* MeshBuilder::GenerateCube(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ)
{
	Vertex v;
	
	std::vector<Vertex> VBO;
	std::vector<unsigned> index_buffer_data;

	v.pos.Set(-lengthX / 2.0f, lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(-lengthX / 2.0f, -lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(lengthX / 2.0f, lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);

	v.pos.Set(lengthX / 2.0f, lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(-lengthX / 2.0f, -lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(lengthX / 2.0f, -lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);

	//side left
	v.pos.Set(-lengthX / 2.0f, lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(-lengthX / 2.0f, -lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(-lengthX / 2.0f, lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);

	v.pos.Set(-lengthX / 2.0f, lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(-lengthX / 2.0f, -lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(-lengthX / 2.0f, -lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);

	//side right
	v.pos.Set(lengthX / 2.0f, lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(lengthX / 2.0f, -lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(lengthX / 2.0f, lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);

	v.pos.Set(lengthX / 2.0f, lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(lengthX / 2.0f, -lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(lengthX / 2.0f, -lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);

	//side Back
	v.pos.Set(lengthX / 2.0f, lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(lengthX / 2.0f, -lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(-lengthX / 2.0f, lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);

	v.pos.Set(-lengthX / 2.0f, lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(lengthX / 2.0f, -lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(-lengthX / 2.0f, -lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);

	//Side top
	v.pos.Set(-lengthX / 2.0f, lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(-lengthX / 2.0f, lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(lengthX / 2.0f, lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);

	v.pos.Set(lengthX / 2.0f, lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(-lengthX / 2.0f, lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(lengthX / 2.0f, lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);

	//Side Bottom
	v.pos.Set(-lengthX / 2.0f, -lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(-lengthX / 2.0f, -lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(lengthX / 2.0f, -lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);

	v.pos.Set(lengthX / 2.0f, -lengthY / 2.0f, lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(-lengthX / 2.0f, -lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);
	v.pos.Set(lengthX / 2.0f, -lengthY / 2.0f, -lengthZ / 2.0f); v.color = color; VBO.push_back(v);

	for (unsigned i = 0; i < VBO.size(); ++i)
	{
		index_buffer_data.push_back(i);
	}

	Mesh *mesh = new Mesh(meshName);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer); //bind index buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(unsigned), &index_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, VBO.size() * sizeof(Vertex), &VBO[0], GL_STATIC_DRAW);



	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;


	return mesh;
}
Mesh* MeshBuilder::GenerateRing(const std::string &meshName, Color color, unsigned int numSlices, float radius, float innerRadius)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;

	float anglePerSlice = 360.0f / numSlices;
	for (unsigned slice = 0; slice <= numSlices; slice++)
	{
		float theta = slice * anglePerSlice;
		v.pos.Set(radius * cos(Math::DegreeToRadian(theta)), 0, radius * sin(Math::DegreeToRadian(theta)));
		v.color = color;
		vertex_buffer_data.push_back(v);

		v.pos.Set(innerRadius * cos(Math::DegreeToRadian(theta)), 0, innerRadius * sin(Math::DegreeToRadian(theta)));
		v.color = color;
		vertex_buffer_data.push_back(v);
	}
	for (unsigned slice = 0; slice <= numSlices; slice++)
	{
		index_buffer_data.push_back(slice * 2);
		index_buffer_data.push_back(slice * 2 + 1);
	}
	Mesh *mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(unsigned), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}
Mesh* MeshBuilder::GenerateSphere(const std::string &meshName, Color color, unsigned int numStacks, unsigned int numSlices, float radius)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;

	float AnglePerstacks = 180.0f / numStacks;
	float AnglePerslice = 360.0f / numSlices;

	for (unsigned int stack = 0; stack <= numStacks; stack++) //stack //replace with 180 for sphere
	{
		float phi = -90.0f + stack * AnglePerstacks;
		for (float theta = 0; theta <= 360; theta += AnglePerslice) //slice
		{
			v.pos.x = radius * cos(Math::DegreeToRadian(phi)) * cos(Math::DegreeToRadian(theta));
			v.pos.y = radius * sin(Math::DegreeToRadian(phi));
			v.pos.z = radius * cos(Math::DegreeToRadian(phi)) * sin(Math::DegreeToRadian(theta));
			v.color = color;
			v.normal.Set(cos(Math::DegreeToRadian(phi)) * cos(Math::DegreeToRadian(theta)), sin(Math::DegreeToRadian(phi)), cos(Math::DegreeToRadian(phi)) * sin(Math::DegreeToRadian(theta)));
			vertex_buffer_data.push_back(v);

		}
	}

	for (unsigned int stack = 0; stack <= numStacks; stack++) 
	{
		for (unsigned slice = 0; slice <= numSlices; slice++)
		{
			index_buffer_data.push_back(stack * (numSlices + 1) + slice);
			index_buffer_data.push_back((stack + 1)* (numSlices + 1) + slice);
		}
	}


	Mesh *mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(unsigned), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}
Mesh* MeshBuilder::GenerateHemisphere(const std::string &meshName, Color color, unsigned int numStacks, unsigned int numSlices, float radius)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;

	float stacks = 90.0f / numStacks;
	float slice = 360.0f / numSlices;

	for (float phi = 90; phi >= 0; phi -= stacks)
	{
		for (float theta = 0; theta <= 360; theta += slice)
		{
			v.pos.x = radius * cos(Math::DegreeToRadian(phi)) * cos(Math::DegreeToRadian(theta));
			v.pos.y = radius * sin(Math::DegreeToRadian(phi));
			v.pos.z = radius * cos(Math::DegreeToRadian(phi)) * sin(Math::DegreeToRadian(theta));
			v.color = color;
			v.normal.Set(cos(Math::DegreeToRadian(phi)) * cos(Math::DegreeToRadian(theta)), sin(Math::DegreeToRadian(phi)), cos(Math::DegreeToRadian(phi)) * sin(Math::DegreeToRadian(theta)));
			vertex_buffer_data.push_back(v);
		}
	}


	for (float theta = 0; theta <= 360; theta += slice)
	{
		v.pos.Set(radius * cos(Math::DegreeToRadian(theta)), 0, radius * sin(Math::DegreeToRadian(theta)));
		v.color = color;
		v.normal.Set(0, -1, 0);
		vertex_buffer_data.push_back(v);
	}

	v.pos.Set(0, 0, 0);
	v.color = color;
	v.normal.Set(0, -1, 0);
	vertex_buffer_data.push_back(v);

	for (unsigned int stack = 0; stack < numStacks + 1; stack++)
	{
		for (unsigned slice = 0; slice < numSlices + 1; slice++)
		{
			index_buffer_data.push_back((stack + 1)* (numSlices + 1) + slice);
			index_buffer_data.push_back(stack * (numSlices + 1) + slice);
		}
	}

	for (unsigned slice = 0; slice < numSlices + 1; slice++)
	{
		index_buffer_data.push_back(numStacks * (numSlices + 1) + numSlices * 2 + 2);
		index_buffer_data.push_back(numStacks * (numSlices + 1) + slice + 1);
	}

	Mesh *mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(unsigned), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}
Mesh* MeshBuilder::GenerateTorus(const std::string &meshName, Color color, unsigned int numStacks, unsigned int numSlices, float innerRadius, float outerRadius)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;


	float degreePerStack = 360.f / numStacks;
	float degreePerSlice = 360.f / numSlices;
	float x1, z1; //y1 is always zero
	float x2, y2, z2;
	for (unsigned stack = 0; stack < numStacks + 1; stack++) {
		for (unsigned slice = 0; slice < numSlices + 1; slice++) {
			z1 = innerRadius * cos(Math::DegreeToRadian(stack * degreePerStack));
			x1 = innerRadius * sin(Math::DegreeToRadian(stack * degreePerStack));

			z2 = (innerRadius + outerRadius * cos(Math::DegreeToRadian(slice * degreePerSlice))) * cos(Math::DegreeToRadian(stack * degreePerStack));
			y2 = outerRadius * sin(Math::DegreeToRadian(slice * degreePerSlice));
			x2 = (innerRadius + outerRadius * cos(Math::DegreeToRadian(slice * degreePerSlice))) * sin(Math::DegreeToRadian(stack * degreePerStack));

			v.pos.Set(x2,y2,z2);
			v.color = color;
			v.normal.Set(x2 - x1, y2, z2 - z1);
			vertex_buffer_data.push_back(v);
			
		}
	}

	for (unsigned stack = 0; stack < numStacks; stack++) {
		for (unsigned slice = 0; slice < numSlices + 1; slice++) {
			index_buffer_data.push_back((numSlices + 1) * stack + slice + 0);
			index_buffer_data.push_back((numSlices + 1) * (stack + 1) + slice + 0);
			}
		}

	Mesh *mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(unsigned), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}
Mesh* MeshBuilder::GenerateCylinder(const std::string &meshName, Color color, unsigned int numSlices, float BaseRadius, float TopRadius, float height)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;
	
	//BOTTOM
	float anglePerSlice = 360.0f / numSlices;
	for (unsigned slice = 0; slice <= numSlices; slice++)
	{
		float theta = slice * anglePerSlice;
		v.pos.Set(BaseRadius * cos(Math::DegreeToRadian(theta)), (-height) * 0.5f, BaseRadius * sin(Math::DegreeToRadian(theta)));
		v.color = color;
		v.normal.Set(0, -1, 0);
		vertex_buffer_data.push_back(v);
	}

	v.pos.Set(0, (-height)*0.5f, 0);
	v.color = color;
	v.normal.Set(0, -1, 0);
	vertex_buffer_data.push_back(v);


	//CURVE
	for (unsigned slice = 0; slice <= numSlices; slice++)
	{
		float theta = slice * anglePerSlice;
		v.pos.Set(BaseRadius * cos(Math::DegreeToRadian(theta)), (-height) * 0.5f, BaseRadius * sin(Math::DegreeToRadian(theta)));
		v.color = color;
		v.normal.Set(cos(Math::DegreeToRadian(theta)), 0, sin(Math::DegreeToRadian(theta)));
		vertex_buffer_data.push_back(v);

		v.pos.Set(TopRadius * cos(Math::DegreeToRadian(theta)), (height)* 0.5f, TopRadius * sin(Math::DegreeToRadian(theta)));
		v.color = color;
		v.normal.Set(cos(Math::DegreeToRadian(theta)), 0, sin(Math::DegreeToRadian(theta)));
		vertex_buffer_data.push_back(v);
	}

	//TOP
	for (unsigned slice = 0; slice <= numSlices; slice++)
	{
		float theta = slice * anglePerSlice;
		v.pos.Set(TopRadius * cos(Math::DegreeToRadian(theta)), (height)* 0.5f, TopRadius * sin(Math::DegreeToRadian(theta)));
		v.color = color;
		if (TopRadius == 0.0f)
		{
			v.normal.Set(height * cos(Math::DegreeToRadian(theta)), BaseRadius, height * sin(Math::DegreeToRadian(theta)));
			v.normal.Normalize();
		}
		else
			v.normal.Set(0, 1, 0);
		vertex_buffer_data.push_back(v);

	}

	v.pos.Set(0, (height) * 0.5f, 0);
	v.color = color;
	v.normal.Set(0, 1, 0);
	vertex_buffer_data.push_back(v);

	for (unsigned slice = 0; slice <= numSlices; slice++)
	{
		index_buffer_data.push_back(numSlices + 1);
		index_buffer_data.push_back(slice);
	}
	for (unsigned slice = 0; slice <= numSlices; slice++)
	{
		index_buffer_data.push_back(numSlices + 2 + slice * 2);
		index_buffer_data.push_back(numSlices + 2 + slice * 2 + 1);
	}
	for (unsigned slice = 0; slice <= numSlices; slice++)
	{
		index_buffer_data.push_back(numSlices + 2 + numSlices * 2 + 1 + slice + 1);
		index_buffer_data.push_back((numSlices + 2) + (numSlices) * 2 + 1 + numSlices + 2);
	}

	Mesh *mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(unsigned), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}
Mesh* MeshBuilder::GenerateOBJ(const std::string &meshName, const std::string &file_path)
{
	//Read vertices, texcoords & normals from OBJ
	std::vector<Position> vertices;
	std::vector<TexCoord> uvs;
	std::vector<Vector3> normals;
	bool success = LoadOBJ(file_path.c_str(), vertices, uvs, normals);
	if (!success)
		return NULL;
	//Index the vertices, texcoords & normals properly
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;
	IndexVBO(vertices, uvs, normals, index_buffer_data, vertex_buffer_data);
	//Create the mesh and call glBindBuffer, glBufferData
	//Use triangle list and remember to set index size

	Mesh *mesh = new Mesh(meshName);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer); //bind index buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}
Mesh* MeshBuilder::GenerateText(const std::string &meshName, unsigned numRow, unsigned numCol)
{
	Vertex v;

	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;

	for (unsigned stringLength = 0; stringLength < meshName.size(); stringLength++)
	{
		float width = 1.f / numCol;
		float height = 1.f / numRow;
		for (unsigned i = 0; i < numRow; ++i)
		{
			for (unsigned j = 0; j < numCol; ++j)
			{

					float lengthX, lengthY, lengthZ;
					lengthX = 1.0f;
					lengthY = 1.0f;
					lengthZ = 0.0f;
//					-lengthX / 2.0f;// = -0
//					lengthX / 2.0f// = 0.5
					Color color(1, 1, 1);

					v.pos.Set(0, 1, 0); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(j * width, (numRow - i - 1) * height + height); vertex_buffer_data.push_back(v);
					v.pos.Set(0, 0, 0); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(j * width, (numRow - i - 1) * height); vertex_buffer_data.push_back(v);
					v.pos.Set(1, 0, 0); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(j * width + width, (numRow - i - 1) * height); vertex_buffer_data.push_back(v);

					v.pos.Set(1, 0, 0); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(j * width + width, (numRow - i - 1) * height); vertex_buffer_data.push_back(v);
					v.pos.Set(1, 1, 0); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(j * width + width, (numRow - i - 1) * height + height); vertex_buffer_data.push_back(v);
					v.pos.Set(0, 1, 0); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(j * width, (numRow - i - 1) * height + height); vertex_buffer_data.push_back(v);

				//Task: Add 4 vertices into vertex_buffer_data

				//Task: Add 6 indices into index_buffer_data

			}
		}
	}
	for (unsigned i = 0; i < vertex_buffer_data.size(); ++i)
	{
		index_buffer_data.push_back(i);
	}

	Mesh *mesh = new Mesh(meshName);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer); //bind index buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(unsigned), &index_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);



	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;


	return mesh;
}
