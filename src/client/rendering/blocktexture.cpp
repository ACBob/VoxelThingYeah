#include "blocktexture.hpp"

BlockTexture GetDefaultBlockTextureSide( BLOCKID type, Direction side )
{
	switch ( type )
	{
		case AIR: // Shouldn't be possible
			return { 0, 0, 0, 0 };
			break;

		case STONE:
			return { 0, 0, 1, 1 };
			break;

		case DIRT:
			return { 1, 0, 1, 1 };
			break;

		case GRASS:
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

		case COBBLE:
			return { 4, 0, 1, 1 };
			break;
		case PLANKS:
			return { 5, 0, 1, 1 };
			break;
		case BEDROCK:
			return { 6, 0, 1, 1 };
			break;
		case GLASS:
			return { 7, 0, 1, 1 };
			break;
		case LOG:
			if ( side == Direction::UP || side == Direction::DOWN )
			{
				return { 8, 1, 1, 1 };
			}
			return { 8, 0, 1, 1 };
			break;
		case LEAVES:
			return { 9, 0, 1, 1 };
			break;
		case WATER:
		case WATERSRC:
			return { 10, 0, 1, 1 };
			break;
		case LAVA:
		case LAVASRC:
			return { 11, 0, 1, 1 };
			break;
		case ORE_COAL:
			return { 12, 0, 1, 1 };
			break;
		case ORE_IRON:
			return { 13, 0, 1, 1 };
			break;
		case FLOWER:
			return { 14, 0, 1, 1 };
			break;
		case SAND:
			return { 15, 0, 1, 1 };
			break;
		case SANDSTONE:
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
		case BRICKS:
			return { 3, 1, 1, 1 };
			break;
		case LGRASS:
			return { 4, 1, 1, 1 };
			break;
		case MOSSCBBLE:
			// Moss grows more in the north
			// Show this by a subtle texture difference
			if ( side == Direction::NORTH )
				return { 5, 1, 1, 1 };
			return { 6, 1, 1, 1 };
			break;
		case SNOW:
			return { 9, 1, 1, 1 };
			break;
		case SNOWGRASS:
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
		case ICE:
			return { 10, 1, 1, 1 };
			break;
		case LIGHT_YELLOW:
			return { 11, 1, 1, 1 };
			break;
		case LIGHT_WHITE:
			return { 12, 1, 1, 1 };
			break;
		case LIGHT_RED:
			return { 13, 1, 1, 1 };
			break;
		case LIGHT_GREEN:
			return { 14, 1, 1, 1 };
			break;
		case LIGHT_BLUE:
			return { 15, 1, 1, 1 };
			break;
		case WOOL:
			return { 0, 2, 1, 1};
			break;
	}
}