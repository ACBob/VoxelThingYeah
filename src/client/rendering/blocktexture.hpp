#include "shared/blockdef.hpp"
#include "texturemanager.hpp"
#include "utility/direction.hpp"

#pragma once

using BlockTexture = Vector4f;

BlockTexture GetDefaultBlockTextureSide( BLOCKID type, Direction side );