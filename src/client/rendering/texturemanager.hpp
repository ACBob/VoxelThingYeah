#pragma once

#include <vector>

#include "utility/types.hpp"

namespace materialSystem
{
	class CTexture
	{
	  public:
		CTexture( const char *path );

		std::vector<unsigned char> m_imageData;
		unsigned int m_iWidth, m_iHeight, m_iId;

		const char *m_cFilePath = nullptr;
	};

	void Init();
	void UnInit();

	CTexture *LoadTexture( const char *path );

	extern std::vector<CTexture *> loadedTextures;
}; // namespace materialSystem

using CTexture = materialSystem::CTexture;