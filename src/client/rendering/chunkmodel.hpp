#include "modelmanager.hpp"
#include "utility/vector.hpp"
#include "world/block.hpp"
#include <vector>

#include "assorted.hpp"

#pragma once

// Forward Decl.
class CChunk;

void BuildChunkModel( CModel &mdl, CModel &wmdl, block_t blocks[], Vector3f pos, CChunk *chunk );