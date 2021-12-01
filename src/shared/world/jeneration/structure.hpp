// Minecraft-like structure system

#pragma once

#include "blockdef.hpp"
#include "utility/direction.hpp"

#include "../world.hpp"

enum StructureType {
	TREE = 0,
	TOWER,
	PYRAMID,
};

#define N AIR
#define L LOG
#define l LEAVES

// A tree
static const BLOCKID TREE_STRUCTURE[6][25] = {
	{ N, N, N, N, N, 
	  N, N, N, N, N,
	  N, N, L, N, N,
	  N, N, N, N, N,
	  N, N, N, N, N },

	{ N, N, N, N, N,
	  N, N, N, N, N,
	  N, N, L, N, N,
	  N, N, N, N, N,
	  N, N, N, N, N },

	{ l, l, l, l, l,
	  l, l, l, l, l,
	  l, l, L, l, l,
	  l, l, l, l, l,
	  l, l, l, l, l },

	{ N, l, l, l, N,
	  l, l, l, l, l,
	  l, l, L, l, l,
	  l, l, l, l, l,
	  N, l, l, l, N },

	{ N, N, l, N, N,
	  N, l, l, l, N,
	  l, l, L, l, l,
	  N, l, l, l, N,
	  N, N, l, N, N },

	{ N, N, N, N, N,
	  N, N, l, N, N,
	  N, l, l, l, N,
	  N, N, l, N, N,
	  N, N, N, N, N },
};

#undef N
#undef L
#undef l

class CStructure
{
  public:
	constexpr CStructure( StructureType type ) : m_type( type ) {}

	void Generate( CWorld *pWorld, int x, int y, int z, Direction dir )
	{
		// TODO: other structures
		// Generate a tree

		// We do this by copying TREE_STRUCTURE to the world
		// Rotation is ignored for now

		// each array is a slice of the structure
		// and starts at the bottom of the structure
		// and goes up
		// Each slice is a 2d array of X by Z

		for ( int iy = 0; iy < 6; iy++ )
		{
			for ( int ix = 0; ix < 5; ix++ )
			{
				for ( int iz = 0; iz < 5; iz++ )
				{
					if ( TREE_STRUCTURE[iy][ix * 5 + iz] != AIR )
					{
						pWorld->SetBlockAtWorldPos( { (float)x + ix, (float)y + iy, (float)z + iz },
													TREE_STRUCTURE[iy][ix * 5 + iz] );
					}
				}
			}
		}
	}

  private:
	StructureType m_type;
};

const CStructure &GetStructure( StructureType type );