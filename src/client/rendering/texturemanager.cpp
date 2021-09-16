#include "texturemanager.hpp"

#include "lodepng.h"

#include <cstdio>
#include <memory>

#include <glad/glad.h>

#include "filesystem.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

std::vector<CTexture *> materialSystem::loadedTextures;

CTexture::CTexture( const char *path )
// TODO: Error Texture
{
	bool bSuccess		= false;
	int64_t iFileLength = 0;
	m_cFilePath			= path;

	const uchar_t *cPNGData = fileSystem::LoadFile( path, iFileLength, bSuccess );
	if ( iFileLength < 10 )
	{
		con_error( "Invalid PNG File!" );
	}

	if ( !bSuccess )
	{
		// TODO: Return with error texture
	}

	uint iError = lodepng::decode( m_imageData, m_iWidth, m_iHeight, cPNGData, iFileLength );
	delete[] cPNGData;

	if ( iError != 0 )
	{
		con_error( "LodePNG Error: %s", lodepng_error_text( iError ) );
	}

	glGenTextures( 1, &m_iId );
	glBindTexture( GL_TEXTURE_2D, m_iId );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &m_imageData[0] );
}
CTexture::CTexture( std::vector<unsigned char> imageData, unsigned int width, unsigned int height )
{
	m_imageData = imageData;
	m_iWidth = width;
	m_iHeight = height;

	glGenTextures( 1, &m_iId );
	glBindTexture( GL_TEXTURE_2D, m_iId );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &m_imageData[0] );
}

void materialSystem::Init() {}

CTexture *materialSystem::LoadTexture( const char *path )
{
	for ( CTexture *t : loadedTextures )
		if ( t->m_cFilePath == path )
			return t;

	CTexture *tex = new CTexture( path );
	loadedTextures.push_back( tex );

	return loadedTextures.back();
}

CTexture *materialSystem::LoadTexture( std::vector<unsigned char> imageData, unsigned int width, unsigned int height )
{
	CTexture *tex = new CTexture( imageData, width, height );
	loadedTextures.push_back( tex );

	return loadedTextures.back();
}

void materialSystem::UnInit()
{
	// Unload textures
	for ( CTexture *t : loadedTextures )
		delete t;
}
