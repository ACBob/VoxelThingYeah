#include "blockbase.hpp"

#include "blockair.hpp"
#include "blockerror.hpp"
#include "blockgeneric.hpp"
#include "blockleaves.hpp"
#include "blockliquid.hpp"
#include "blocksoundmaker.hpp"
#include "blocktransgeneric.hpp"
#include "blockwool.hpp"
#include "blockplant.hpp"

#include "world/chunk.hpp"

#include "entities/entitybase.hpp"

CBoundingBox CBlockBase::GetBounds() const
{
	return CBoundingBox( Vector3f( 0 ), Vector3f( BLOCKUNIT, BLOCKUNIT, BLOCKUNIT ), Vector3f( 0 ) );
}

#ifdef CLIENTEXE
bool CBlockBase::FaceVisible( Direction direction, BLOCKID blockId ) const
{
	return BlockType( blockId ).IsTransparent() || !BlockType( blockId ).IsFullCube();
}
#endif

// I am *some-what* copying Cuberite here.
namespace
{
	constexpr CBlockAir _BlockAir( AIR );
	constexpr CBlockGeneric _BlockStone( STONE );

	// TODO: Seperate block_t variants for each block type
	constexpr CBlockGeneric _BlockDirt( DIRT );
	constexpr CBlockGeneric _BlockGrass( GRASS );
	constexpr CBlockGeneric _BlockCobble( COBBLE );
	constexpr CBlockGeneric _BlockPlanks( PLANKS );
	constexpr CBlockGeneric _BlockBedrock( BEDROCK );
	constexpr CBlockTransGeneric _BlockGlass( GLASS );
	constexpr CBlockGeneric _BlockLog( LOG );
	constexpr CBlockLeaves _BlockLeaves( LEAVES );
	constexpr CBlockLiquid _BlockWater( WATER );
	constexpr CBlockLiquid _BlockWaterSRC( WATERSRC );
	constexpr CBlockLiquid _BlockLava( LAVA );
	constexpr CBlockLiquid _BlockLavaSrc( LAVASRC );
	constexpr CBlockGeneric _BlockOreCoal(
		ORE_COAL ); // These should probably have a generic CBlockOre, with some kind of "alloy" or "metal" to drop
	constexpr CBlockGeneric _BlockOreIron( ORE_IRON );
	constexpr CBlockPlant _BlockFlower(
		FLOWER ); // CBlockFlower should be generic, use the data value to determine which type of flower
	constexpr CBlockGeneric _BlockSand( SAND );
	constexpr CBlockGeneric _BlockSandStone( SANDSTONE );
	constexpr CBlockGeneric _BlockBricks( BRICKS );
	constexpr CBlockPlant _BlockLongGrass( LGRASS );
	constexpr CBlockGeneric _BlockMossyCobble( MOSSCBBLE );
	constexpr CBlockGeneric _BlockSnow( SNOW ); // TODO: Do layers of snow like minecraft?
	constexpr CBlockGeneric _BlockSnowyGrass(
		SNOWGRASS ); // TODO: Data value of BlockGrass, instead of a separate block
	constexpr CBlockTransGeneric _BlockIce( ICE );
	constexpr CBlockGeneric _BlockLightYellow(
		LIGHT_YELLOW ); // TODO: Should be a generic CBlockLight, with a data value of the color
	constexpr CBlockGeneric _BlockLightWhite( LIGHT_WHITE );
	constexpr CBlockGeneric _BlockLightRed( LIGHT_RED );
	constexpr CBlockGeneric _BlockLightGreen( LIGHT_GREEN );
	constexpr CBlockGeneric _BlockLightBlue( LIGHT_BLUE );
	constexpr CBlockWool _BlockWool( WOOL );
	constexpr CBlockSoundMaker _BlockSoundMaker( SOUNDMAKER );
	constexpr CBlockGeneric _BlockStoneBricks( STONEBRICKS );
	constexpr CBlockGeneric _BlockGravel( GRAVEL );
	constexpr CBlockGeneric _BlockIronBlock( IRONBLOCK );
	constexpr CBlockGeneric _BlockBookshelf( BOOKSHELF );
	constexpr CBlockGeneric _BlockTNT( TNT );

	constexpr CBlockError _BlockInvalid( AIR );
} // namespace

const CBlockBase &BlockType( BLOCKID type )
{
	switch ( type )
	{
		case AIR:
			return _BlockAir;
		case STONE:
			return _BlockStone;
		case DIRT:
			return _BlockDirt;
		case GRASS:
			return _BlockGrass;
		case COBBLE:
			return _BlockCobble;
		case PLANKS:
			return _BlockPlanks;
		case BEDROCK:
			return _BlockBedrock;
		case GLASS:
			return _BlockGlass;
		case LOG:
			return _BlockLog;
		case LEAVES:
			return _BlockLeaves;
		case WATER:
			return _BlockWater;
		case WATERSRC:
			return _BlockWaterSRC;
		case LAVA:
			return _BlockLava;
		case LAVASRC:
			return _BlockLavaSrc;
		case ORE_COAL:
			return _BlockOreCoal;
		case ORE_IRON:
			return _BlockOreIron;
		case FLOWER:
			return _BlockFlower;
		case SAND:
			return _BlockSand;
		case SANDSTONE:
			return _BlockSandStone;
		case BRICKS:
			return _BlockBricks;
		case LGRASS:
			return _BlockLongGrass;
		case MOSSCBBLE:
			return _BlockMossyCobble;
		case SNOW:
			return _BlockSnow;
		case SNOWGRASS:
			return _BlockSnowyGrass;
		case ICE:
			return _BlockIce;
		case LIGHT_YELLOW:
			return _BlockLightYellow;
		case LIGHT_WHITE:
			return _BlockLightWhite;
		case LIGHT_RED:
			return _BlockLightRed;
		case LIGHT_GREEN:
			return _BlockLightGreen;
		case LIGHT_BLUE:
			return _BlockLightBlue;
		case WOOL:
			return _BlockWool;
		case SOUNDMAKER:
			return _BlockSoundMaker;
		case STONEBRICKS:
			return _BlockStoneBricks;
		case GRAVEL:
			return _BlockGravel;
		case IRONBLOCK:
			return _BlockIronBlock;
		case BOOKSHELF:
			return _BlockBookshelf;
		case TNT:
			return _BlockTNT;
		default:
			return _BlockInvalid;
	}
}