#include <iostream>
#include <fstream>
#include <map>

#include "LoadOBJ.h"
#include "Vector3.h"

bool LoadOBJ(
	const char *file_path, 
	std::vector<Position> & out_vertices, 
	std::vector<TexCoord> & out_uvs, 
	std::vector<Vector3> & out_normals,
	Vector3& out_HitboxMin, Vector3& out_HitboxMax
)
{
	//Fill up code from OBJ lecture notes
	std::ifstream fileStream(file_path, std::ios::binary);
	if (!fileStream.is_open())
	{
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ? \n";
			return false;
	}

	std::vector<unsigned> vertexIndices, uvIndices, normalIndices;
	std::vector<Position> temp_vertices;
	std::vector<TexCoord> temp_uvs;
	std::vector<Vector3> temp_normals;

	//assuming that the object is always on the origin and there are vertices smaller/bigger than 0,0,0
	Vector3 temp_min(0, 0, 0);
	Vector3 temp_max(0, 0, 0);


	while (!fileStream.eof()) {
		char buf[256];
		fileStream.getline(buf, 256);
		if (strncmp("v ", buf, 2) == 0) 
		{
			//process vertex position
			Position vertex;
			sscanf_s((buf + 2), "%f%f%f", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);

			if (vertex.x < temp_min.x)
				temp_min.x = vertex.x;
			if (vertex.y < temp_min.y)
				temp_min.y = vertex.y;
			if (vertex.z < temp_min.z)
				temp_min.z = vertex.z;

			if (vertex.x > temp_max.x)
				temp_max.x = vertex.x;
			if (vertex.y > temp_max.y)
				temp_max.y = vertex.y;
			if (vertex.z > temp_max.z)
				temp_max.z = vertex.z;
		}
		else if (strncmp("vt ", buf, 3) == 0) 
		{ //process texcoord
			TexCoord texCoord;
			sscanf_s((buf + 2), "%f%f%f", &texCoord.u, &texCoord.v);
			temp_uvs.push_back(texCoord);
		}
		else if (strncmp("vn ", buf, 3) == 0) 
		{ //process normal
			Vector3 normal;
			sscanf_s((buf + 2), "%f%f%f", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strncmp("f ", buf, 2) == 0) 
		{ //process face

			unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
			int matches = sscanf_s((buf + 2), "%d/%d/%d %d/%d/%d %d/%d/%d %d / %d / %d \n",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2],
				&vertexIndex[3], &uvIndex[3], &normalIndex[3]);

			if (matches == 9) //triangle
			{
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
			else if (matches == 12)
			{
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);

				vertexIndices.push_back(vertexIndex[2]);
				vertexIndices.push_back(vertexIndex[3]);
				vertexIndices.push_back(vertexIndex[0]);
				uvIndices.push_back(uvIndex[2]);
				uvIndices.push_back(uvIndex[3]);
				uvIndices.push_back(uvIndex[0]);
				normalIndices.push_back(normalIndex[2]);
				normalIndices.push_back(normalIndex[3]);
				normalIndices.push_back(normalIndex[0]);
			}
			else 
			{
				std::cout << "Error line: " << buf << std::endl;
				std::cout << "File can't be read by parser\n";
				return false;

			}
		}
	}
	fileStream.close();


	for (unsigned i = 0; i < vertexIndices.size(); ++i)
	{
		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		// Get the attributes thanks to the index
		Position vertex = temp_vertices[vertexIndex - 1];
		TexCoord uv = temp_uvs[uvIndex - 1];
		Vector3 normal = temp_normals[normalIndex - 1];
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);
	}
	out_HitboxMin = temp_min;
	out_HitboxMax = temp_max;

	return true;
}

struct PackedVertex{
	Position position;
	TexCoord uv;
	Vector3 normal;
	bool operator<(const PackedVertex that) const{
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
	};
};

bool getSimilarVertexIndex_fast( 
	PackedVertex & packed, 
	std::map<PackedVertex, unsigned short> & VertexToOutIndex,
	unsigned short & result
){
	std::map<PackedVertex, unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if(it == VertexToOutIndex.end())
	{
		return false;
	}
	else
	{
		result = it->second;
		return true;
	}
}

void IndexVBO(
	std::vector<Position> & in_vertices,
	std::vector<TexCoord> & in_uvs,
	std::vector<Vector3> & in_normals,

	std::vector<unsigned> & out_indices,
	std::vector<Vertex> & out_vertices
)
{
	std::map<PackedVertex,unsigned short> VertexToOutIndex;

	// For each input vertex
	for(unsigned int i = 0; i < in_vertices.size(); ++i) 
	{

		PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};

		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);

		if ( found )
		{ 
			// A similar vertex is already in the VBO, use it instead !
			out_indices.push_back( index );
		}
		else
		{ 
			// If not, it needs to be added in the output data.
			Vertex v;
			v.pos.Set(in_vertices[i].x, in_vertices[i].y, in_vertices[i].z);
			v.texCoord.Set(in_uvs[i].u, in_uvs[i].v);
			v.normal.Set(in_normals[i].x, in_normals[i].y, in_normals[i].z);
			v.color.Set(1, 1, 1);
			out_vertices.push_back(v);
			unsigned newindex = (unsigned)out_vertices.size() - 1;
			out_indices.push_back( newindex );
			VertexToOutIndex[ packed ] = newindex;
		}
	}
}