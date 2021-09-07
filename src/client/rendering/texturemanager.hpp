#pragma once

#include <vector>

#include "utility/types.hpp"

namespace materialSystem
{
	class CTexture
	{
	  public:
		CTexture( const char *path );

		std::vector<unsigned char> image;
		unsigned int width, height, id;

		const char *fp = nullptr;
	};

	void Init();
	void UnInit();

	CTexture *LoadTexture( const char *path );

	extern std::vector<CTexture *> loadedTextures;
}; // namespace materialSystem

using CTexture = materialSystem::CTexture;