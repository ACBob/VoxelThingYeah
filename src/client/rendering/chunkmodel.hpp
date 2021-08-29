// TODO: genric model class

#include "utility/vector.hpp"
#include "world/block.hpp"
#include "model.hpp"
#include <vector>

#pragma once

void BuildChunkModel(Model &m, Block blocks[], Vector pos, void *chunk);