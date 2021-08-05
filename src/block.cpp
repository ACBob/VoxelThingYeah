#include "block.h"

#include <stdio.h>

Block::Block()
{

}

BlockTexture Block::GetSideTexture(blockface_t Side)
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
			if (Side == blockface_t::UP)
			{
				return {2,0, 1,1};
			}
			if (Side == blockface_t::DOWN)
			{
				return {1,0, 1,1};
			}
			return {3,0, 1,1};
		break;
	}
}