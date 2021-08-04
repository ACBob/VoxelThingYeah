// TODO: Seperate texture class?
#pragma once

#include <vector>

typedef unsigned short Texture;

class TextureManager
{
	public:
		Texture LoadTexture(const char* path);

		std::vector<Texture> loadedTextures;
};