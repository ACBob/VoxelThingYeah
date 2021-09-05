#include "rendering/chunkmodel.hpp"

#include "world/chunk.hpp" // CHUNKSIZE definitions
#include "world/world.hpp"

#include <iostream>

int indexArray(int x, int y, int z)
{
	return CHUNK3D_TO_1D(x,y,z);
}

const Model::Vertex cubeVertices[] = {
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
const Model::Vertex plantVertices[] = {
	{  0        ,  BLOCKUNIT,  0        },
	{  BLOCKUNIT,  0        ,  BLOCKUNIT},
	{  0        ,  0        ,  0        },
	{  BLOCKUNIT,  BLOCKUNIT,  BLOCKUNIT},

	{  BLOCKUNIT,  BLOCKUNIT,  0        },
	{  0        ,  0        ,  BLOCKUNIT},
	{  BLOCKUNIT,  0        ,  0        },
	{  0        ,  BLOCKUNIT,  BLOCKUNIT},
};

// D-d-d-d-double vector
const std::vector<std::vector<int>> cubeTris = {
	{3, 2, 1, 0}, // N
	{6, 3, 0, 5}, // E
	{7, 6, 5, 4}, // S
	{2, 7, 4, 1}, // W

	{0, 1, 4, 5}, // UP
	{6, 7, 2, 3} // DN
};

const std::vector<int> plantTris = {
	2, 1, 3, 0,
	6, 5, 7, 4
};

std::vector<Model::Vertex> sampleCubeFace(Direction dir, Block block, int x = 0, int y = 0, int z = 0)
{
	std::vector<Model::Vertex> g;
	for (int i = 0; i < 4; i++)
	{
		g.push_back(cubeVertices[cubeTris[dir][i]]);

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

std::vector<Model::Vertex> samplePlant(Block block, int x = 0, int y = 0, int z = 0)
{
	std::vector<Model::Vertex> g;
	for (int i = 0; i < 8; i++)
	{
		g.push_back(plantVertices[plantTris[i]]);

		g[i].x += x;
		g[i].y += y;
		g[i].z += z;

		Vector normal = DirectionVector[i > 4 ? SOUTH : NORTH];

		g[i].nx = normal.x;
		g[i].ny = normal.y;
		g[i].nz = normal.z;

		BlockTexture tex = block.GetSideTexture(NORTH);

		switch(i)
		{
			case 0:
			case 4:
				g[i].u = (float)tex.x / 16.0f;
				g[i].v = (float)tex.y / 16.0f;
			break;
			case 1:
			case 5:
				g[i].u = (float)(tex.x + tex.sizex) / 16.0f;
				g[i].v = (float)tex.y / 16.0f;
			break;
			case 2:
			case 6:
				g[i].u = (float)(tex.x + tex.sizex) / 16.0f;
				g[i].v = (float)(tex.y + tex.sizey) / 16.0f;
			break;
			case 3:
			case 7:
				g[i].u = (float)tex.x / 16.0f;
				g[i].v = (float)(tex.y + tex.sizey) / 16.0f;
			break;
		}
	}

	return g;
}

// We include the chunk manager here so we can test our neighbouring chunks
void BuildChunkModel(Model &mdl, Block blocks[], Vector pos, void *chunk)
{
	mdl.vertices.clear();
	mdl.faces.clear();

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
					BlockFeatures blockFeatures = GetBlockFeatures(block.blockType);
					switch(blockFeatures.model)
					{
						case BLOCKMODEL_CUBE:
							for (int i = 0; i < 6; i++)
							{
								Vector neighbour = Vector(x,y,z) + DirectionVector[i];
								if (ValidChunkPosition(neighbour))
								{
									blocktype_t blockType = reinterpret_cast<Chunk*>(chunk)->GetBlockAtLocal(neighbour)->blockType;
									BlockFeatures bF = GetBlockFeatures(blockType);
									if (bF.rule == OBSCURERULE_ALWAYS ||
										(bF.rule == OBSCURERULE_SIMILAR && blockType == block.blockType))
										continue;
								}
								else
								{
									// Test a neighbour
									Chunk *chunkNeighbour = reinterpret_cast<Chunk*>(chunk)->Neighbour(Direction(i));
									if (chunkNeighbour != nullptr)
									{
										neighbour = neighbour + (DirectionVector[i] * -16.0f);

										Block *b = chunkNeighbour->GetBlockAtLocal(neighbour);
										if (b == nullptr)
											continue;
										BlockFeatures bF = GetBlockFeatures(b->blockType);
										if (bF.rule == OBSCURERULE_ALWAYS ||
											(bF.rule == OBSCURERULE_SIMILAR && b->blockType == block.blockType))
											continue;
									}
								}

								std::vector<Model::Vertex> g = sampleCubeFace(Direction(i), block, x, y, z);
								std::copy(g.begin(), g.end(), std::back_inserter(mdl.vertices));

								int nVertices = mdl.vertices.size();

								mdl.faces.push_back(
									{nVertices - 4, nVertices - 3, nVertices - 2}
								);
								mdl.faces.push_back(
									{nVertices - 4, nVertices - 2, nVertices - 1}
								);
							}
						break;

						case BLOCKMODEL_PLANT:
							std::vector<Model::Vertex> g = samplePlant(block, x, y, z);
							std::copy(g.begin(), g.end(), std::back_inserter(mdl.vertices));

							int nVertices = mdl.vertices.size();

							mdl.faces.push_back(
								{nVertices - 4, nVertices - 3, nVertices - 2}
							);
							mdl.faces.push_back(
								{nVertices - 4, nVertices - 2, nVertices - 1}
							);
							mdl.faces.push_back(
								{nVertices - 2, nVertices - 3, nVertices - 4}
							);
							mdl.faces.push_back(
								{nVertices - 1, nVertices - 2, nVertices - 4}
							);

							mdl.faces.push_back(
								{nVertices - 8, nVertices - 7, nVertices - 6}
							);
							mdl.faces.push_back(
								{nVertices - 8, nVertices - 6, nVertices - 5}
							);
							mdl.faces.push_back(
								{nVertices - 6, nVertices - 7, nVertices - 8}
							);
							mdl.faces.push_back(
								{nVertices - 5, nVertices - 6, nVertices - 8}
							);
						break;
					}
				}
			}
		}
	}

	mdl.Update();
}