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

TextureManager::TextureManager()
{
}

Texture *TextureManager::LoadTexture(const char* path)
{
	Texture *tex = new Texture(path);
	loadedTextures.push_back(tex);

	return loadedTextures.back();
}
TextureManager::~TextureManager()
{
	// Unload textures
	for (Texture *t : loadedTextures)
		delete t;
}
