#include "blockdef.hpp"

MATERIAL GetBlockMaterial( BLOCKID blockType )
{
	switch ( blockType )
	{
		default:
		case BLCK_STONE:
		case BLCK_COBBLE:
		case BLCK_BEDROCK:
		case BLCK_COALORE:
		case BLCK_IRONORE:
		case BLCK_SANDSTONE:
		case BLCK_BRICKS:
			return MATERIAL::MAT_STONE;
			break;

		case BLCK_DIRT:
			return MATERIAL::MAT_LOOSE;
			break;

		case BLCK_PLANKS:
		case BLCK_LOG:
			return MATERIAL::MAT_WOOD;
			break;

		case BLCK_GLASS:
		case BLCK_ICE:
		case LIGHT_RED:
		case LIGHT_BLUE:
		case LIGHT_GREEN:
		case LIGHT_WHITE:
		case LIGHT_YELLOW:
			return MATERIAL::MAT_GLASS;
			break;

		case BLCK_GRASS:
		case BLCK_LEAVES:
		case BLCK_FLOWER:
		case BLCK_TALLGRASS:
		case BLCK_SNOWGRASS:
			return MATERIAL::MAT_ORGANIC;
			break;

		case BLCK_WATER:
		case BLCK_WATERSRC:
		case BLCK_LAVA:
		case BLCK_LAVASRC:
			return MATERIAL::MAT_LIQUID;
			break;

		case BLCK_SAND:
		case BLCK_SNOW:
			return MATERIAL::MAT_DUST;
			break;
		
		case BLCK_WOOL:
			return MATERIAL::MAT_FABRIC;
			break;
	}
}

const char *BlockMaterialSTR( MATERIAL mat )
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
	bF.liquidFlow	  = BLCK_AIR;
	bF.liquidSource	  = BLCK_AIR;
	bF.liquidRange	  = 0;
	bF.liquidSpeed	  = 0;
	bF.rule			  = OBSCURERULE_ALWAYS;
	bF.model		  = BLOCKMODEL_CUBE;

	switch ( blockType )
	{
		default:
			break;

		case BLCK_BEDROCK:
			bF.breakable = false;
			break;

		case BLCK_LEAVES:
		case BLCK_GLASS:
		case BLCK_ICE:
			bF.rule		  = blockType == BLCK_LEAVES ? OBSCURERULE_NEVER : OBSCURERULE_SIMILAR;
			bF.solid	  = false;
			bF.opaqueness = blockType == BLCK_ICE ? 0x110 : blockType == BLCK_LEAVES ? 0x444 : 0x111;
			break;

		case BLCK_FLOWER:
		case BLCK_TALLGRASS:
			bF.rule		  = OBSCURERULE_NEVER;
			bF.solid	  = false;
			bF.model	  = BLOCKMODEL_PLANT;
			bF.opaqueness = 0x000; // doesn't block light
			break;

		case BLCK_WATERSRC:
		case BLCK_LAVASRC:
			bF.isLiquidSource = true;
		case BLCK_WATER:
		case BLCK_LAVA:
			bF.breakable	 = false;
			bF.selectable	 = false;
			bF.walkable		 = false;
			bF.solid		 = false;
			bF.isLiquid		 = true;
			bF.isLightSource = blockType == BLCK_LAVA || blockType == BLCK_LAVASRC;
			bF.lightColour	 = ( blockType == BLCK_LAVA || blockType == BLCK_LAVASRC ) ? 0xEA3 : 0x000;
			bF.liquidSpeed	 = blockType == BLCK_LAVA ? 15 : 5;
			bF.rule			 = OBSCURERULE_SIMILAR;
			bF.liquidFlow	 = ( blockType == BLCK_WATERSRC || blockType == BLCK_WATER ) ? BLCK_WATER : BLCK_LAVA;
			bF.liquidSource	 = ( blockType == BLCK_WATERSRC || blockType == BLCK_WATER ) ? BLCK_WATERSRC : BLCK_LAVASRC;
			bF.liquidRange	 = ( blockType == BLCK_WATERSRC || blockType == BLCK_WATER ) ? 7 : 4;
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
		
		case BLCK_WOOL:
			bF.colouration    = BLOCKCOLOURATION_16BIT;
			break;

		case BLCK_AIR:
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