// TODO: genric model class

#include "modelmanager.hpp"
#include "utility/vector.hpp"
#include "world/block.hpp"
#include <vector>

#pragma once

void BuildChunkModel( Model &m, Block blocks[], Vector pos, void *chunk );