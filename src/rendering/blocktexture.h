#include "texturemanager.h"
#include "utility/direction.h"
#include "world/blockdef.h"

#pragma once

struct BlockTexture
{
	BlockTexture(int x, int y, int sizex, int sizey)
	{
		this->x=x;
		this->y=y;
		this->sizex=sizex;
		this->sizey=sizey;
	};

	int x, y, sizex, sizey;
};

BlockTexture GetDefaultBlockTextureSide(blocktype_t type, Direction side);