#include "blockdef.hpp"

blockmaterial_t GetBlockMaterial( BLOCKID blockType )
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
		case ICE:
		case LIGHT_RED:
		case LIGHT_BLUE:
		case LIGHT_GREEN:
		case LIGHT_WHITE:
		case LIGHT_YELLOW:
			return blockmaterial_t::MAT_GLASS;
			break;

		case GRASS:
		case LEAVES:
		case FLOWER:
		case LGRASS:
		case SNOWGRASS:
			return blockmaterial_t::MAT_ORGANIC;
			break;

		case WATER:
		case WATERSRC:
		case LAVA:
		case LAVASRC:
			return blockmaterial_t::MAT_LIQUID;
			break;

		case SAND:
		case SNOW:
			return blockmaterial_t::MAT_DUST;
			break;
		
		case WOOL:
			return blockmaterial_t::MAT_FABRIC;
			break;
	}
}

const char *BlockMaterialSTR( blockmaterial_t mat )
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
	}
}

BlockFeatures GetBlockFeatures( BLOCKID blockType )
{
	BlockFeatures bF;
	bF.breakable	  = true;
	bF.selectable	  = true;
	bF.solid		  = true;
	bF.walkable		  = true;
	bF.floodable	  = false;
	bF.isLiquid		  = false;
	bF.isLiquidSource = false;
	bF.isLightSource  = false;
	bF.colouration    = BLOCKCOLOURATION_NONE;
	bF.lightColour	  = 0x000;
	bF.opaqueness	  = 0xFFF;
	bF.liquidFlow	  = AIR;
	bF.liquidSource	  = AIR;
	bF.liquidRange	  = 0;
	bF.liquidSpeed	  = 0;
	bF.rule			  = OBSCURERULE_ALWAYS;
	bF.model		  = BLOCKMODEL_CUBE;

	switch ( blockType )
	{
		default:
			break;

		case BEDROCK:
			bF.breakable = false;
			break;

		case LEAVES:
		case GLASS:
		case ICE:
			bF.rule		  = blockType == LEAVES ? OBSCURERULE_NEVER : OBSCURERULE_SIMILAR;
			bF.solid	  = false;
			bF.opaqueness = blockType == ICE ? 0x110 : blockType == LEAVES ? 0x444 : 0x111;
			break;

		case FLOWER:
		case LGRASS:
			bF.rule		  = OBSCURERULE_NEVER;
			bF.solid	  = false;
			bF.model	  = BLOCKMODEL_PLANT;
			bF.opaqueness = 0x000; // doesn't block light
			break;

		case WATERSRC:
		case LAVASRC:
			bF.isLiquidSource = true;
		case WATER:
		case LAVA:
			bF.breakable	 = false;
			bF.selectable	 = false;
			bF.walkable		 = false;
			bF.solid		 = false;
			bF.isLiquid		 = true;
			bF.isLightSource = blockType == LAVA || blockType == LAVASRC;
			bF.lightColour	 = ( blockType == LAVA || blockType == LAVASRC ) ? 0xEA3 : 0x000;
			bF.liquidSpeed	 = blockType == LAVA ? 15 : 5;
			bF.rule			 = OBSCURERULE_SIMILAR;
			bF.liquidFlow	 = ( blockType == WATERSRC || blockType == WATER ) ? WATER : LAVA;
			bF.liquidSource	 = ( blockType == WATERSRC || blockType == WATER ) ? WATERSRC : LAVASRC;
			bF.liquidRange	 = ( blockType == WATERSRC || blockType == WATER ) ? 7 : 4;
			bF.opaqueness	 = 0x221;
			break;

		case LIGHT_RED:
		case LIGHT_GREEN:
		case LIGHT_BLUE:
		case LIGHT_YELLOW:
		case LIGHT_WHITE:
			bF.isLightSource = true;
			bF.lightColour =
				blockType == LIGHT_WHITE
					? 0xFFF
					: blockType == LIGHT_YELLOW
						  ? 0xFD9
						  : blockType == LIGHT_RED
								? 0xF00
								: blockType == LIGHT_GREEN ? 0x0F0 : blockType == LIGHT_BLUE ? 0x00F : 0xF0F;
			bF.opaqueness = 0x000;
			break;
		
		case WOOL:
			bF.colouration    = BLOCKCOLOURATION_16BIT;
			break;

		case AIR:
			bF.breakable  = false;
			bF.selectable = false;
			bF.solid	  = false;
			bF.walkable	  = false;
			bF.floodable  = true;
			bF.rule		  = OBSCURERULE_NEVER;
			bF.opaqueness = 0x000; // doesn't block light
			break;
	}

	return bF;
}