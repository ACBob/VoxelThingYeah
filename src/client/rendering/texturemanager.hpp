#pragma once

#include <vector>

#include "utility/types.hpp"

namespace materialSystem
{
	class CTexture
	{
	  public:
		CTexture( const char *path );
		CTexture( std::vector<unsigned char> imageData, unsigned int height, unsigned int width );

		std::vector<unsigned char> m_imageData;
		unsigned int m_iWidth, m_iHeight, m_iId;

		char *m_cFilePath = nullptr;
	};

	void Init();
	void UnInit();

	CTexture *LoadTexture( const char *path );
	CTexture *LoadTexture( std::vector<unsigned char> imageData, unsigned int height, unsigned int width );

	extern std::vector<CTexture *> loadedTextures;
}; // namespace materialSystem

using CTexture = materialSystem::CTexture;