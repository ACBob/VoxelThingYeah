#include "chunkmodel.h"
#include "chunk.h" // CHUNKSIZE definitions

#include <iostream>

int indexArray(int x, int y, int z)
{
	return int(x + CHUNKSIZE_X * (y + CHUNKSIZE_Z * z));
}
const ChunkModel::Vertex vertices[] = {
	// NORTH +Z
	{ 1,  1, 1},
	{ 0,  1, 1},
	{ 0,  0, 1},
	{ 1,  0, 1},
	// SOUTH -Z
	{ 0,  1,  0},
	{ 1,  1,  0},
	{ 1,  0,  0},
	{ 0,  0,  0},	
};

// D-d-d-d-double vector
const std::vector<std::vector<int>> triangles = {
	{0, 1, 2, 3}, // N
	{5, 0, 3, 6}, // E
	{4, 5, 6, 7}, // S
	{1, 4, 7, 2}, // W

	{5, 4, 1, 0}, // UP
	{3, 2, 7, 6} // DN
};

std::vector<ChunkModel::Vertex> sampleFace(blockface_t dir, int x = 0, int y = 0, int z = 0)
{
	std::vector<ChunkModel::Vertex> g;
	for (int i = 0; i < 4; i++)
	{
		g.push_back(vertices[triangles[dir][i]]);

		g[i].x += x;
		g[i].y += y;
		g[i].z += z;
	}

	return g;
}


void ChunkModel::Build(blocktype_t blocks[])
{
	vertices.clear();
	faces.clear();

	for (int y = 0; y < CHUNKSIZE_Y; y++)
	{
		for (int x = 0; x < CHUNKSIZE_X; x++)
		{
			for (int z = 0; z < CHUNKSIZE_Z; z++)
			{
				// blocktype_t block = blocks[indexArray(x,y,z)];

				// block here! Construct!
				for (int i = 0; i < 6; i++)
				{
					std::vector<Vertex> g = sampleFace(blockface_t(i), x, y, z);
					std::copy(g.begin(), g.end(), std::back_inserter(vertices));

					int nVertices = vertices.size();

					faces.push_back(
						{nVertices - 4, nVertices - 3, nVertices - 2}
					);
					faces.push_back(
						{nVertices - 4, nVertices - 2, nVertices - 1}
					);
				}
			}
		}
	}
}