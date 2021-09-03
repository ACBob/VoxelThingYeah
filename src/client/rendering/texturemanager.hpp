#pragma once

#include <vector>

#include "utility/types.hpp"

namespace materialSystem
{
	class Texture
	{
		public:
			Texture(const char* path);

			std::vector<unsigned char> image;
			unsigned int width, height, id;
	};

	void Init();
	void UnInit();

	Texture* LoadTexture(const char* path);

	extern std::vector<Texture*> loadedTextures;
};

using Texture = materialSystem::Texture;