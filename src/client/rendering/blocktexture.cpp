#include "blocktexture.hpp"

BlockTexture GetDefaultBlockTextureSide( BLOCKID type, Direction side )
{
	switch ( type )
	{
		case BLCK_AIR: // Shouldn't be possible
			return { 0, 0, 0, 0 };
			break;

		case BLCK_STONE:
			return { 0, 0, 1, 1 };
			break;

		case BLCK_DIRT:
			return { 1, 0, 1, 1 };
			break;

		case BLCK_GRASS:
			if ( side == Direction::UP )
			{
				return { 2, 0, 1, 1 };
			}
			if ( side == Direction::DOWN )
			{
				return { 1, 0, 1, 1 };
			}
			return { 3, 0, 1, 1 };
			break;

		case BLCK_COBBLE:
			return { 4, 0, 1, 1 };
			break;
		case BLCK_PLANKS:
			return { 5, 0, 1, 1 };
			break;
		case BLCK_BEDROCK:
			return { 6, 0, 1, 1 };
			break;
		case BLCK_GLASS:
			return { 7, 0, 1, 1 };
			break;
		case BLCK_LOG:
			if ( side == Direction::UP || side == Direction::DOWN )
			{
				return { 8, 1, 1, 1 };
			}
			return { 8, 0, 1, 1 };
			break;
		case BLCK_LEAVES:
			return { 9, 0, 1, 1 };
			break;
		case BLCK_WATER:
		case BLCK_WATERSRC:
			return { 10, 0, 1, 1 };
			break;
		case BLCK_LAVA:
		case BLCK_LAVASRC:
			return { 11, 0, 1, 1 };
			break;
		case BLCK_COALORE:
			return { 12, 0, 1, 1 };
			break;
		case BLCK_IRONORE:
			return { 13, 0, 1, 1 };
			break;
		case BLCK_FLOWER:
			return { 14, 0, 1, 1 };
			break;
		case BLCK_SAND:
			return { 15, 0, 1, 1 };
			break;
		case BLCK_SANDSTONE:
			if ( side == Direction::UP )
			{
				return { 0, 1, 1, 1 };
			}
			if ( side == Direction::DOWN )
			{
				return { 2, 1, 1, 1 };
			}
			return { 1, 1, 1, 1 };
			break;
		case BLCK_BRICKS:
			return { 3, 1, 1, 1 };
			break;
		case BLCK_TALLGRASS:
			return { 4, 1, 1, 1 };
			break;
		case BLCK_MOSSYCOBBLE:
			// Moss grows more in the north
			// Show this by a subtle texture difference
			if ( side == Direction::NORTH )
				return { 5, 1, 1, 1 };
			return { 6, 1, 1, 1 };
			break;
		case BLCK_SNOW:
			return { 9, 1, 1, 1 };
			break;
		case BLCK_SNOWGRASS:
			if ( side == Direction::UP )
			{
				return { 9, 1, 1, 1 };
			}
			if ( side == Direction::DOWN )
			{
				return { 1, 0, 1, 1 };
			}
			return { 7, 1, 1, 1 };
			break;
		case BLCK_ICE:
			return { 10, 1, 1, 1 };
			break;
		case BLCK_WOOL:
			return { 0, 2, 1, 1};
			break;
	}
}