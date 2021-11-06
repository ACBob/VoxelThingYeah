#include "blockbase.hpp"

#include "blockair.hpp"
#include "blockstone.hpp"
#include "blockerror.hpp"
#include "blockliquid.hpp"

#include "world/chunk.hpp"

#include "entities/entitybase.hpp"

CBoundingBox CBlockBase::GetBounds() const {
    return CBoundingBox( CVector(0), CVector( BLOCKUNIT, BLOCKUNIT, BLOCKUNIT ), CVector( 0 ) );
}

#ifdef CLIENTEXE
bool CBlockBase::FaceVisible( Direction direction, BLOCKID blockId ) const
{
    return !BlockType( blockId ).IsSolid(0);
}
#endif

// I am *some-what* copying Cuberite here.
namespace
{
    constexpr CBlockAir _BlockAir(AIR);
    constexpr CBlockStone _BlockStone(STONE);

    // TODO: Seperate CBlock variants for each block type
    constexpr CBlockStone _BlockDirt(DIRT);
    constexpr CBlockStone _BlockGrass(GRASS);
    constexpr CBlockStone _BlockCobble(COBBLE);
    constexpr CBlockStone _BlockPlanks(PLANKS);
    constexpr CBlockStone _BlockBedrock(BEDROCK);
    constexpr CBlockStone _BlockGlass(GLASS);
    constexpr CBlockStone _BlockLog(LOG);
    constexpr CBlockStone _BlockLeaves(LEAVES);
    constexpr CBlockLiquid _BlockWater(WATER);
    constexpr CBlockLiquid _BlockWaterSRC(WATERSRC);
    constexpr CBlockLiquid _BlockLava(LAVA);
    constexpr CBlockLiquid _BlockLavaSrc(LAVASRC);
    constexpr CBlockStone _BlockOreCoal(ORE_COAL); // These should probably have a generic CBlockOre, with some kind of "alloy" or "metal" to drop
    constexpr CBlockStone _BlockOreIron(ORE_IRON);
    constexpr CBlockStone _BlockFlower(FLOWER); // CBlockFlower should be generic, use the data value to determine which type of flower
    constexpr CBlockStone _BlockSand(SAND);
    constexpr CBlockStone _BlockSandStone(SANDSTONE);
    constexpr CBlockStone _BlockBricks(BRICKS);
    constexpr CBlockStone _BlockLongGrass(LGRASS);
    constexpr CBlockStone _BlockMossyCobble(MOSSCBBLE);
    constexpr CBlockStone _BlockSnow(SNOW); // TODO: Do layers of snow like minecraft?
    constexpr CBlockStone _BlockSnowyGrass(SNOWGRASS); // TODO: Data value of BlockGrass, instead of a separate block
    constexpr CBlockStone _BlockIce(ICE);
    constexpr CBlockStone _BlockLightYellow(LIGHT_YELLOW); // TODO: Should be a generic CBlockLight, with a data value of the color
    constexpr CBlockStone _BlockLightWhite(LIGHT_WHITE);
    constexpr CBlockStone _BlockLightRed(LIGHT_RED);
    constexpr CBlockStone _BlockLightGreen(LIGHT_GREEN);
    constexpr CBlockStone _BlockLightBlue(LIGHT_BLUE);
    constexpr CBlockStone _BlockWool(WOOL);

    constexpr CBlockError _BlockInvalid(AIR);
}

const CBlockBase &BlockType( BLOCKID type ) {
    switch( type ) {
        case AIR: return _BlockAir;
        case STONE: return _BlockStone;
        case DIRT: return _BlockDirt;
        case GRASS: return _BlockGrass;
        case COBBLE: return _BlockCobble;
        case PLANKS: return _BlockPlanks;
        case BEDROCK: return _BlockBedrock;
        case GLASS: return _BlockGlass;
        case LOG: return _BlockLog;
        case LEAVES: return _BlockLeaves;
        case WATER: return _BlockWater;
        case WATERSRC: return _BlockWaterSRC;
        case LAVA: return _BlockLava;
        case LAVASRC: return _BlockLavaSrc;
        case ORE_COAL: return _BlockOreCoal;
        case ORE_IRON: return _BlockOreIron;
        case FLOWER: return _BlockFlower;
        case SAND: return _BlockSand;
        case SANDSTONE: return _BlockSandStone;
        case BRICKS: return _BlockBricks;
        case LGRASS: return _BlockLongGrass;
        case MOSSCBBLE: return _BlockMossyCobble;
        case SNOW: return _BlockSnow;
        case SNOWGRASS: return _BlockSnowyGrass;
        case ICE: return _BlockIce;
        case LIGHT_YELLOW: return _BlockLightYellow;
        case LIGHT_WHITE: return _BlockLightWhite;
        case LIGHT_RED: return _BlockLightRed;
        case LIGHT_GREEN: return _BlockLightGreen;
        case LIGHT_BLUE: return _BlockLightBlue;
        case WOOL: return _BlockWool;
        default: return _BlockInvalid;
    }
}