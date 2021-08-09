#include "rendering/chunkmodel.h"

#include "world/chunk.h" // CHUNKSIZE definitions
#include "world/chunkmanager.h"

#include <iostream>

int indexArray(int x, int y, int z)
{
	return CHUNK3D_TO_1D(x,y,z);
}

ChunkModel::ChunkModel(void* chunk)
{
	_chunk = chunk;
}

const ChunkModel::Vertex vertices[] = {
	// NORTH +Z
	{  BLOCKUNIT,  BLOCKUNIT,  BLOCKUNIT},
	{  0        ,  BLOCKUNIT,  BLOCKUNIT},
	{  0        ,  0        ,  BLOCKUNIT},
	{  BLOCKUNIT,  0        ,  BLOCKUNIT},
	// SOUTH -Z
	{  0        ,  BLOCKUNIT,  0        },
	{  BLOCKUNIT,  BLOCKUNIT,  0        },
	{  BLOCKUNIT,  0        ,  0        },
	{  0        ,  0        ,  0        },
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

std::vector<ChunkModel::Vertex> sampleFace(Direction dir, Block block, int x = 0, int y = 0, int z = 0)
{
	std::vector<ChunkModel::Vertex> g;
	for (int i = 0; i < 4; i++)
	{
		g.push_back(vertices[triangles[dir][i]]);

		g[i].x += x;
		g[i].y += y;
		g[i].z += z;

		Vector normal = DirectionVector[dir];

		g[i].nx = normal.x;
		g[i].ny = normal.y;
		g[i].nz = normal.z;

		BlockTexture tex = block.GetSideTexture(dir);

		switch(i)
		{
			case 0:
				g[i].u = (float)tex.x / 16.0f;
				g[i].v = (float)tex.y / 16.0f;
			break;
			case 1:
				g[i].u = (float)(tex.x + tex.sizex) / 16.0f;
				g[i].v = (float)tex.y / 16.0f;
			break;
			case 2:
				g[i].u = (float)(tex.x + tex.sizex) / 16.0f;
				g[i].v = (float)(tex.y + tex.sizey) / 16.0f;
			break;
			case 3:
				g[i].u = (float)tex.x / 16.0f;
				g[i].v = (float)(tex.y + tex.sizey) / 16.0f;
			break;
		}
	}

	return g;
}

// We include the chunk manager here so we can test our neighbouring chunks
void ChunkModel::Build(Block blocks[], Vector pos)
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
				Block block = blocks[indexArray(x,y,z)];

				// block here! Construct!
				if (block.blockType != AIR)
				{
					for (int i = 0; i < 6; i++)
					{
						// TODO: Test against other chunks
						Vector neighbour = Vector(x,y,z) + DirectionVector[i];
						if (ValidChunkPosition(neighbour))
						{
							if (
								reinterpret_cast<Chunk*>(_chunk)->GetBlockAtLocal(neighbour)->blockType != AIR
							) // Skip if neighbouring
							{
								continue;
							}
						}
						else
						{
							// TODO: Fix this
							// Test a neighbour
							Chunk *chunkNeighbour = reinterpret_cast<Chunk*>(_chunk)->Neighbour(Direction(i));
							if (chunkNeighbour != nullptr)
							{
								neighbour = neighbour + (DirectionVector[i] * -16.0f);

								Block *b = chunkNeighbour->GetBlockAtLocal(neighbour);
								if (b != nullptr && b->blockType != AIR)
									continue;
							}
						}

						std::vector<Vertex> g = sampleFace(Direction(i), block, x, y, z);
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

	Update();
}