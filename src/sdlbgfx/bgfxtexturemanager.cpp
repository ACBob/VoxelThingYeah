#include "../texturemanager.h"

#include "lodepng.h"

#include <bgfx/bgfx.h>

#include <cstdio>
#include <memory>

Texture TextureManager::LoadTexture(const char* path)
{
	// TODO: Caching?
	

	std::vector<unsigned char> image;
	uint width, height;
	uint err = lodepng::decode(image, width, height, path);

	if (err != 0)
	{
		printf("LodePNG Error: %s\n", lodepng_error_text(err));
		return NULL; // TODO: Error texture
	}

	const bgfx::Memory* mem = bgfx::alloc(image.size());
	memcpy(mem->data, image.data(), image.size());

	// TODO: I *think* clamp keeps the texture from sampling outside its' UV, I'm not sure
	bgfx::TextureHandle tex = bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::Enum::RGBA8, BGFX_SAMPLER_POINT | BGFX_SAMPLER_U_CLAMP, mem);

	loadedTextures.push_back(reinterpret_cast<Texture>(tex.idx));

	return reinterpret_cast<Texture>(tex.idx);
}