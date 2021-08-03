#include "chunkmodel.h"
#include "chunk.h" // CHUNKSIZE definitions

#include <iostream>

int indexArray(int x, int y, int z)
{
	return int(x + CHUNKSIZE_X * (y + CHUNKSIZE_Z * z));
}

void ChunkModel::Build(blocktype_t blocks[])
{
	vertices.clear();
	faces.clear();

	// vertices.reserve(CHUNKSIZE_X*CHUNKSIZE_Y*CHUNKSIZE_Z*4);

	int j = 0;
	for (int x = 0; x < CHUNKSIZE_X; x++)
	{
		for (int y = 0; y < CHUNKSIZE_Y; y++)
		{
			for (int z = 0; z < CHUNKSIZE_Z; z++)
			{
				// blocktype_t block = blocks[indexArray(x,y,z)];

				// block here! Construct!
				vertices.push_back(
					{x + -1.0f, y +  1.0f, z +  1.0f}
				);
				vertices.push_back(
					{x +  1.0f, y +  1.0f, x +  1.0f}
				);
				vertices.push_back(
					{x + -1.0f, y + -1.0f, x +  1.0f}
				);
				vertices.push_back(
					{x +  1.0f, y + -1.0f, x +  1.0f}
				);

				j ++;
			}
		}
	}
	std::cout << j << std::endl;

	for (int i = 0; i < vertices.size() / 4; i+=4)
	{
		faces.push_back(Face(i, i + 1, i + 2));
		faces.push_back(Face(i + 1, i + 3, i + 2));
	}
}