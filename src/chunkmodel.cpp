#include "chunkmodel.h"
#include "chunk.h" // CHUNKSIZE definitions

#include <iostream>

int indexArray(int x, int y, int z)
{
	return x * CHUNKSIZE_Y * CHUNKSIZE_Z + y * CHUNKSIZE_Z + z;
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
	{3, 2, 1, 0}, // N
	{6, 3, 0, 5}, // E
	{7, 6, 5, 4}, // S
	{2, 7, 4, 1}, // W

	{0, 1, 4, 5}, // UP
	{6, 7, 2, 3} // DN
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

// TODO: Somewhere else?
Vector blockdir[] = {
	{0, 0, 1}, // NORTH
	{1, 0, 0}, // EAST
	{0, 0, -1}, // SOUTH
	{-1, 0, 0}, // WEST

	{0, 1, 0}, // UP
	{0, -1, 0} // DOWN
};

void ChunkModel::Build(blocktype_t blocks[])
{
	vertices.clear();
	faces.clear();

	// Reserve enough space to have verticies inside all space
	vertices.reserve(CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z * 4);

	for (int y = 0; y < CHUNKSIZE_Y; y++)
	{
		for (int x = 0; x < CHUNKSIZE_X; x++)
		{
			for (int z = 0; z < CHUNKSIZE_Z; z++)
			{
				blocktype_t block = blocks[indexArray(x,y,z)];

				// block here! Construct!
				if (block == true)
				{
					for (int i = 0; i < 6; i++)
					{
						// TODO: Test against other chunks
						Vector neighbour = Vector(x,y,z) + blockdir[i];
						if ( ValidChunkPosition(neighbour) )
						{
							if (
								blocks[indexArray(neighbour.x,neighbour.y,neighbour.z)]
							) // Skip if neighbouring
							{
								continue;
							}
						}

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
}