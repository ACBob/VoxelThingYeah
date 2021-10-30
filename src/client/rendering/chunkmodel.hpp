#include "modelmanager.hpp"
#include "utility/vector.hpp"
#include "world/block.hpp"
#include <vector>

#include "assorted.hpp"

#pragma once

void BuildChunkModel( CModel &mdl, CModel &wmdl, BLOCKID blocks[], int16_t meta[], CVector pos, void *chunk );