#include "modelmanager.hpp"
#include "utility/vector.hpp"
#include "blockdef.hpp"
#include <vector>

#include "assorted.hpp"

#pragma once

void BuildChunkModel( CModel &mdl, CModel &wmdl, BLOCKID blocks[], BLOCKVAL meta[], CVector pos, void *chunk );