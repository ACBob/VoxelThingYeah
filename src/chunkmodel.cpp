#include "chunkmodel.h"
#include "chunk.h" // CHUNKSIZE definitions

int indexArray(int x, int y, int z)
{
	return int(x + CHUNKSIZE_X * (y + CHUNKSIZE_Z * z));
}

ChunkModel::Face* quad(int i)
{
	ChunkModel::Face faces[2] = {
		ChunkModel::Face(i, i + 1, i + 2),
		ChunkModel::Face(i + 1, i + 3, i + 2)
	};

	return faces;
}

void ChunkModel::Build(blocktype_t blocks[])
{
	vertices.clear();
	faces.clear();

	// vertices.reserve(CHUNKSIZE_X*CHUNKSIZE_Y*CHUNKSIZE_Z*4);

	// for (int x = 0; x < CHUNKSIZE_X; x++)
	// {
	// 	for (int y = 0; y < CHUNKSIZE_Y; y++)
	// 	{
	// 		for (int z = 0; z < CHUNKSIZE_Z; z++)
	// 		{
	// 			blocktype_t block = blocks[indexArray(x,y,z)];

	// 			if (block)
	// 			{
	// 				// block here! Construct!
					
	// 			}
	// 		}
	// 	}
	// }
	float x, y, z = 0;

	vertices.push_back(
		{-1.0f,  1.0f,  1.0f}
	);
	vertices.push_back(
		{ 1.0f,  1.0f,  1.0f}
	);
	vertices.push_back(
		{-1.0f, -1.0f,  1.0f}
	);
	vertices.push_back(
		{ 1.0f, -1.0f,  1.0f}
	);

	// int j = 0;
	// for (int i = 0; i < (vertices.size() - 1) / 4; i+=4)
	// {
	// int i = 0;
	// Face *a = quad(i);
	int i = 0;
	faces.push_back(
		{0, 1, 2}
	);
	faces.push_back(
		{1, 3, 2}
	);
	// }
}