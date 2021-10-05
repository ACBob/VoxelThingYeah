#include "modelmanager.hpp"
#include "utility/vector.hpp"
#include "world/block.hpp"
#include <vector>

#include "assorted.hpp"

#pragma once

void BuildChunkModel( CModel &mdl, CModel &wmdl, CBlock blocks[], CVector pos, void *chunk );