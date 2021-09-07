// TODO: genric model class

#include "modelmanager.hpp"
#include "utility/vector.hpp"
#include "world/block.hpp"
#include <vector>

#pragma once

void BuildChunkModel( CModel &m, CBlock blocks[], CVector pos, void *chunk );