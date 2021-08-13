#include "blocktexture.h"

BlockTexture GetDefaultBlockTextureSide(blocktype_t type, Direction side)
{
	switch (type)
	{
		case AIR: // Shouldn't be possible
			return {0,0,0,0};
		break;

		case STONE:
			return {0,0, 1,1};
		break;

		case DIRT:
			return {1,0, 1,1};
		break;

		case GRASS:
			if (side == Direction::UP)
			{
				return {2,0, 1,1};
			}
			if (side == Direction::DOWN)
			{
				return {1,0, 1,1};
			}
			return {3,0, 1,1};
		break;

		case COBBLE:
			return {4,0, 1,1};
		break;
		case PLANKS:
			return {5,0, 1,1};
		break;
		case BEDROCK:
			return {6,0, 1,1};
		break;
		case GLASS:
			return {7,0, 1,1};
		break;
	}
}