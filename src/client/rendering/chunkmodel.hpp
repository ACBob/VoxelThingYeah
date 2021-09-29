#include "modelmanager.hpp"
#include "utility/vector.hpp"
#include "world/block.hpp"
#include <vector>

#pragma once

void BuildChunkModel( CModel &m, CModel &wm, CBlock blocks[], CVector pos, void *chunk );