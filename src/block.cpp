#include "block.h"

Block::Block()
{

}

BlockTexture Block::GetSideTexture(blockface_t Side)
{
	switch (this->blockType)
	{
		case AIR:
			return {0,0,0,0};
		break;

		case STONE:
			return {0,0, 16,16};
		break;

		case DIRT:
			return {16,0, 16,16};
		break;

		case GRASS:
			if (Side == blockface_t::UP)
			{
				return {32,0, 16,16};
			}
			if (Side == blockface_t::DOWN)
			{
				return {16,0, 16,16};
			}
			return {64,0, 16,16};
		break;
	}
}