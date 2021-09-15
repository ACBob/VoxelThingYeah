#include "blockdef.hpp"

blockmaterial_t GetBlockMaterial( blocktype_t blockType )
{
	switch ( blockType )
	{
		default:
		case STONE:
		case COBBLE:
		case BEDROCK:
		case ORE_COAL:
		case ORE_IRON:
		case SANDSTONE:
		case BRICKS:
			return blockmaterial_t::MAT_STONE;
			break;

		case DIRT:
			return blockmaterial_t::MAT_LOOSE;
			break;

		case PLANKS:
		case LOG:
			return blockmaterial_t::MAT_WOOD;
			break;

		case GLASS:
			return blockmaterial_t::MAT_GLASS;
			break;

		case GRASS:
		case LEAVES:
		case FLOWER:
		case LGRASS:
			return blockmaterial_t::MAT_ORGANIC;
			break;

		case WATER:
		case LAVA:
			return blockmaterial_t::MAT_LIQUID;
			break;

		case SAND:
			return blockmaterial_t::MAT_DUST;
			break;
	}
}

BlockFeatures GetBlockFeatures( blocktype_t blockType )
{
	BlockFeatures bF;
	bF.breakable   = true;
	bF.selectable  = true;
	bF.solid	   = true;
	bF.walkable	   = true;
	bF.floodable   = false;
	bF.isLiquid	   = false;
	bF.liquidSpeed = 0;
	bF.rule		   = OBSCURERULE_ALWAYS;
	bF.model	   = BLOCKMODEL_CUBE;

	switch ( blockType )
	{
		default:
			break;

		case BEDROCK:
			bF.breakable = false;
			break;

		case LEAVES:
		case GLASS:
			bF.rule	 = blockType == LEAVES ? OBSCURERULE_NEVER : OBSCURERULE_SIMILAR;
			bF.solid = false;
			break;

		case FLOWER:
		case LGRASS:
			bF.rule	 = OBSCURERULE_NEVER;
			bF.solid = false;
			bF.model = BLOCKMODEL_PLANT;
			break;

		case WATER:
		case LAVA:
			bF.breakable   = false;
			bF.selectable  = false;
			bF.walkable	   = false;
			bF.solid	   = false;
			bF.isLiquid	   = true;
			bF.liquidSpeed = blockType == LAVA ? 15 : 5;
			bF.rule		   = OBSCURERULE_SIMILAR;
			break;

		case AIR:
			bF.breakable  = false;
			bF.selectable = false;
			bF.solid	  = false;
			bF.walkable	  = false;
			bF.floodable  = true;
			bF.rule		  = OBSCURERULE_NEVER;
			break;
	}

	return bF;
}