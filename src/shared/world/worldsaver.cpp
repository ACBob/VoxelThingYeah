#include "worldsaver.hpp"

#include "shared/logging.hpp"
#include "world.hpp"

#include "chunk.hpp"

#include "../shared/blockdef.hpp"

#include <cstring>

#include <algorithm>

bool worldIO::saveWorld( CWorld *pWorld, const char *filename )
{
	con_error("World Saving not yet implemented");
	return false;
}

bool worldIO::loadWorld( CWorld *world, const char *filename )
{
	con_error("World Loading not yet implemented");
	return false;
}