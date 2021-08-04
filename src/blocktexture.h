#include "texturemanager.h"

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