#include "texturemanager.h"

#include "lodepng.h"

#include <cstdio>
#include <memory>

#include <glad/glad.h>

Texture::Texture(const char* path)
{
	uint err = lodepng::decode(image, width, height, path);

	if (err != 0)
	{
		printf("LodePNG Error: %s\n", lodepng_error_text(err));
		// TODO: replace data with error data
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
}

// Texture::~Texture()
// {
// }

// TODO: hard-coded limit because of how fragile things are
// You think this is bad? Look at the ChunkManager!
#define MAX_TEXTURES 32

TextureManager::TextureManager()
{
	loadedTextures.reserve(MAX_TEXTURES);
}

Texture *TextureManager::LoadTexture(const char* path)
{
	loadedTextures.emplace_back(path);

	return &loadedTextures.back();
}