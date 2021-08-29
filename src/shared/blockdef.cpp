#include "blockdef.hpp"

blockmaterial_t GetBlockMaterial(blocktype_t blockType)
{
	switch(blockType)
	{
		default:
		case STONE:
		case COBBLE:
		case BEDROCK:
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
			return blockmaterial_t::MAT_ORGANIC;
		break;
	}
}

BlockFeatures GetBlockFeatures(blocktype_t blockType)
{
	BlockFeatures bF;
	bF.breakable = true;
	bF.selectable = true;
	bF.solid = true;
	bF.walkable = true;
	bF.floodable = false;
	bF.rule = OBSCURERULE_ALWAYS;

	switch(blockType)
	{
		default:
		break;

		case BEDROCK:
			bF.breakable = false;
		break;

		case LEAVES:
		case GLASS:
			bF.rule = blockType == LEAVES ? OBSCURERULE_NEVER : OBSCURERULE_SIMILAR;
			bF.solid = false;
		break;

		case WATER:
			bF.breakable = false;
			bF.selectable = false;
			bF.walkable = false;
			bF.solid = false;
		break;
		
		case AIR:
			bF.breakable = false;
			bF.selectable = false;
			bF.solid = false;
			bF.walkable = false;
			bF.floodable = true;
			bF.rule = OBSCURERULE_NEVER;
		break;
	}

	return bF;
}