#include "ModuleEntity.h"

#include <vector>

#include "Globals.h"

using namespace std;

ModuleEntity::ModuleEntity()
{}

ModuleEntity::~ModuleEntity()
{}

bool ModuleEntity::Init()
{
	SetUpCube();
	SetUpSphere();
	return true;
}

bool ModuleEntity::CleanUp()
{
	RELEASE(cube);
	RELEASE(sphere);
	return true;
}

MeshEntity * ModuleEntity::GetCube()
{
	return cube;
}

MeshEntity * ModuleEntity::GetSphere()
{
	return sphere;
}


void ModuleEntity::SetUpCube()
{
	vector<Vertex> vertices;
	vector<GLushort> indices;
	GetCubeMeshData(vertices, indices);

	GLenum drawMode = GL_TRIANGLES;

	cube = new MeshEntity();
	cube->SetMeshData(vertices,indices, drawMode);
}

void ModuleEntity::SetUpSphere()
{
	sphere = new MeshEntity();
	//sphere->SetUpSphere();
}

void ModuleEntity::GetCubeMeshData(vector<Vertex>& vertices, vector<GLushort>& indices) const
{
	assert(vertices.size() == 0);
	assert(indices.size() == 0);

	//Vertices
	{
		const size_t nCubeVertices = 24;
		vertices.resize(nCubeVertices);

		float3 positions[nCubeVertices] = {
			{ -0.5f, -0.5f, -0.5f }, // 0 Front bottom left
		{ 0.5f, -0.5f, -0.5f }, // 1 Front bottom right
		{ 0.5f, 0.5f, -0.5f }, // 2 Front top right
		{ -0.5f, 0.5f, -0.5f }, // 3 Front top left

		{ -0.5f, -0.5f, 0.5f }, // 4 Left bottom left
		{ -0.5f, -0.5f, -0.5f }, // 5 Left bottom right
		{ -0.5f, 0.5f, -0.5f }, // 6 Left top right
		{ -0.5f, 0.5f, 0.5f }, // 7 Left top left

		{ -0.5f, 0.5f, 0.5f }, // 8 Back top left
		{ 0.5f, 0.5f, 0.5f }, // 9 Back top right
		{ 0.5f, -0.5f, 0.5f }, // 10 Back bottom right
		{ -0.5f, -0.5f, 0.5f }, // 11 Back bottom left

		{ 0.5f, -0.5f, -0.5f }, // 12 Right bottom left
		{ 0.5f, -0.5f, 0.5f }, // 13 Right bottom right
		{ 0.5f, 0.5f, 0.5f }, // 14 Right top right
		{ 0.5f, 0.5f, -0.5f }, // 15 Right top left

		{ -0.5f, -0.5f, 0.5f }, // 16 Bottom top left
		{ 0.5f, -0.5f, 0.5f }, // 17 Bottom top right
		{ 0.5f, -0.5f, -0.5f }, // 18 Bottom bottom right
		{ -0.5f, -0.5f, -0.5f }, // 19 Bottom bottom left

		{ -0.5f, 0.5f, -0.5f }, // 20 Top bottom left
		{ 0.5f, 0.5f, -0.5f }, // 21 Top bottom right
		{ 0.5f, 0.5f, 0.5f }, // 22 Top top right
		{ -0.5f, 0.5f, 0.5f } // 23 Top top left
		};

		float4 colors[nCubeVertices];
		for (size_t i = 0; i < nCubeVertices; i++)
		{
			colors[i] = float4(100, 100, 100, 100);
		}

		float2 uv[nCubeVertices] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },

			{ 1.0f, 0.0f },
			{ 0.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },

			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },

			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 0.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		float3 normals[nCubeVertices]; // TODO FILL THE NORMALS WITH THE NORMAL VALUE
		/*float offset = 3;
		for(size_t i = 0; i < 36; i+=3)
		{
		int x1 = indicesArray[i] * offset;
		int x2 = indicesArray[i + 1] * offset;
		int x3 = indicesArray[i + 2] * offset;

		int y1 = (indicesArray[i] * offset) + 1;
		int y2 = (indicesArray[i + 1] * offset) + 1;
		int y3 = (indicesArray[i + 2] * offset) + 1;

		int z1 = (indicesArray[i] * offset) + 2;
		int z2 = (indicesArray[i + 1] * offset) + 2;
		int z3 = (indicesArray[i + 2] * offset) + 2;

		float edge1X = uniqueVertex[x2] - uniqueVertex[x1];
		float edge1Y = uniqueVertex[y2] - uniqueVertex[y1];
		float edge1Z = uniqueVertex[z2] - uniqueVertex[z1];

		float edge2X = uniqueVertex[x3] - uniqueVertex[x1];
		float edge2Y = uniqueVertex[y3] - uniqueVertex[y1];
		float edge2Z = uniqueVertex[z3] - uniqueVertex[z1];

		float3 vNormal = Cross(float3(edge1X, edge1Y, edge1Z), float3(edge2X, edge2Y, edge2Z)).Normalized();
		normal[i] = vNormal.x;
		normal[i + 1] = vNormal.y;
		normal[i + 2] = vNormal.z;
		}*/

		FillVerticesData(vertices, positions, normals, colors, uv);
	}

	//Indices
	{
		const size_t nCubeIndices = 36;
		indices.reserve(nCubeIndices);
		indices = {
			3, 1, 0,
			3, 2, 1,

			7, 5, 4,
			7, 6, 5,

			11, 9, 8,
			11, 10, 9,

			15, 13, 12,
			15, 14, 13,

			19, 17, 16,
			19, 18, 17,

			23, 21, 20,
			23, 22, 21
		};
	}
}
	

void ModuleEntity::FillVerticesData(vector<Vertex>& vertices, const float3 * positions, const float3* normals, const float4 * colors, const float2 * uvs) const
{
	assert(vertices.size() > 0);
	assert(positions);
	assert(normals);
	assert(colors);
	assert(uvs);

	for (GLuint i = 0; i < vertices.size(); ++i)
	{
		vertices[i].position = positions[i];
		vertices[i].normal = normals[i];
		vertices[i].color = colors[i];
		vertices[i].uv = uvs[i];
	}
}