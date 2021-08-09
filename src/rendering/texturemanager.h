// TODO: Seperate texture class?
#pragma once

#include <vector>

class Texture
{
	public:
		Texture(const char* path);

		std::vector<unsigned char> image;
		unsigned int width, height, id;
};

class TextureManager
{
	public:
		TextureManager();

		Texture* LoadTexture(const char* path);

		std::vector<Texture*> loadedTextures;
};