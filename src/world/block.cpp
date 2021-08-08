#include "block.h"
#include "chunk.h"

#include <stdio.h>

Block::Block() :
	chunk(nullptr)
{

}

void Block::Update()
{
	if (chunk != nullptr)
		reinterpret_cast<Chunk*>(chunk)->RebuildMdl();
}

BlockTexture Block::GetSideTexture(Direction Side)
{
	switch (this->blockType)
	{
		case AIR: // Shouldn't be possible
			printf("Something somewhere just tried to get the texture of AIR. Call a programmer!\n");
			return {0,0,0,0};
		break;

		case STONE:
			return {0,0, 1,1};
		break;

		case DIRT:
			return {1,0, 1,1};
		break;

		case GRASS:
			if (Side == Direction::UP)
			{
				return {2,0, 1,1};
			}
			if (Side == Direction::DOWN)
			{
				return {1,0, 1,1};
			}
			return {3,0, 1,1};
		break;

		case COBBLE:
			return {4,0, 1,1};
		break;
	}
}