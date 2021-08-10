#include "texturemanager.h"

#include "lodepng.h"

#include <cstdio>
#include <memory>

#include <glad/glad.h>

#include "physfs.h"

Texture::Texture(const char* path)
		// TODO: Error Texture
{
	PHYSFS_File *f = PHYSFS_openRead(path);
	int64_t fl = PHYSFS_fileLength(f);
	unsigned char buf[fl];

	int64_t rl = PHYSFS_readBytes(f, &buf, fl);

	if (rl != fl)
	{
		printf("PHYSFS Error!\n%s\n", PHYSFS_getLastError());
	}

	uint err = lodepng::decode(image, width, height, buf, fl);

	PHYSFS_close(f);

	if (err != 0)
	{
		printf("LodePNG Error: %s\n", lodepng_error_text(err));
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
