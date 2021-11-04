#include "modelmanager.hpp"
#include "utility/vector.hpp"
#include "blockdef.hpp"
#include <vector>

#include "assorted.hpp"

#pragma once

// Forward decl.
class CChunk;

void BuildChunkModel( CModel &mdl, CModel &wmdl, BLOCKID blocks[], BLOCKVAL meta[], CChunk *chunk );