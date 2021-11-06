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
	}
}