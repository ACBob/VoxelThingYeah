#include "blockdef.hpp"

const char *BlockMaterialSTR( BLOCKMATERIAL mat )
{
	switch ( mat )
	{
		case MAT_NONE:
		default:
			return "none";
		case MAT_STONE:
			return "stone";
		case MAT_LOOSE:
			return "loose";
		case MAT_WOOD:
			return "wood";
		case MAT_GLASS:
			return "glass";
		case MAT_ORGANIC:
			return "organic";
		case MAT_LIQUID:
			return "liquid";
		case MAT_DUST:
			return "dust";
		case MAT_FABRIC:
			return "fabric";
		case MAT_METAL:
			return "metal";
	}
}

BLOCKMATERIAL GetBlockMaterial( BLOCKID id )
{
	switch ( id )
	{
		default:
		case STONE:
		case COBBLE:
		case BEDROCK:
		case ORE_COAL:
		case ORE_IRON:
		case SANDSTONE:
		case BRICKS:
		case STONEBRICKS:
			return MAT_STONE;
			break;

		case DIRT:
			return MAT_LOOSE;
			break;

		case PLANKS:
		case LOG:
		case BOOKSHELF:
			return MAT_WOOD;
			break;

		case GLASS:
		case ICE:
		case LIGHT_RED:
		case LIGHT_BLUE:
		case LIGHT_GREEN:
		case LIGHT_WHITE:
		case LIGHT_YELLOW:
			return MAT_GLASS;
			break;

		case GRASS:
		case LEAVES:
		case FLOWER:
		case LGRASS:
		case SNOWGRASS:
			return MAT_ORGANIC;
			break;

		case WATER:
		case WATERSRC:
		case LAVA:
		case LAVASRC:
			return MAT_LIQUID;
			break;

		case SAND:
		case SNOW:
		case GRAVEL:
		case SNOWLAYER:
			return MAT_DUST;
			break;

		case SOUNDMAKER:
		case WOOL:
		case TNT:
			return MAT_FABRIC;
			break;

		case IRONBLOCK:
			return MAT_METAL;
			break;
	}
}
