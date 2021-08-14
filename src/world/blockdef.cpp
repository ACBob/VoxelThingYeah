#include "blockdef.h"

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

	switch(blockType)
	{
		default:
		break;

		case BEDROCK:
			bF.breakable = false;
		break;

		case GLASS:
		case LEAVES:
			bF.solid = false;
		break;
		
		case AIR:
			bF.breakable = false;
			bF.selectable = false;
			bF.solid = false;
			bF.walkable = false;
		break;
	}

	return bF;
}