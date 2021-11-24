#include "texturemanager.hpp"

#include "lodepng.h"

#include <cstdio>
#include <memory>

#include <glad/glad.h>

#include "filesystem.hpp"

#include "logging.hpp"

std::vector<CTexture *> materialSystem::loadedTextures;

CTexture::CTexture( const char *path )
{
	m_cFilePath = new char[strlen( path ) + 1];
	strcpy( m_cFilePath, path );

	// The image data is RGBA
	// So the data can be written like this for a classic source-style checkerboard
	m_imageData = {
		255, 0, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 0, 255, 255,
	};

	m_iWidth  = 2;
	m_iHeight = 2;

	glGenTextures( 1, &m_iId );
	glBindTexture( GL_TEXTURE_2D, m_iId );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &m_imageData[0] );

	Load();
}
CTexture::CTexture( std::vector<unsigned char> imageData, unsigned int width, unsigned int height )
{
	m_imageData = imageData;
	m_iWidth	= width;
	m_iHeight	= height;

	glGenTextures( 1, &m_iId );
	glBindTexture( GL_TEXTURE_2D, m_iId );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &m_imageData[0] );
}

void CTexture::Load()
{
	bool bSuccess		= false;
	int64_t iFileLength = 0;

	m_imageData.clear();

	const uchar_t *cPNGData = fileSystem::LoadFile( m_cFilePath, iFileLength, bSuccess );
	uint iError				= lodepng::decode( m_imageData, m_iWidth, m_iHeight, cPNGData, iFileLength );
	delete[] cPNGData;
	if ( iFileLength < 10 || !bSuccess || iError != 0 )
	{
		if ( iError != 0 )
		{
			con_error( "LodePNG Error: %s", lodepng_error_text( iError ) );
		}

		con_error( "Invalid PNG %s, loading error texture...", m_cFilePath );

		// The image data is RGBA
		// So the data can be written like this for a classic source-style checkerboard
		m_imageData = {
			255, 0, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 0, 255, 255,
		};

		m_iWidth  = 2;
		m_iHeight = 2;
	}

	glBindTexture( GL_TEXTURE_2D, m_iId );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &m_imageData[0] );
}

void CTexture::Save( const char *path )
{
	if ( path == nullptr )
	{
		path = m_cFilePath;
	}

	std::vector<unsigned char> imageData;
	uint iError = lodepng::encode( imageData, m_imageData, m_iWidth, m_iHeight );
	if ( iError != 0 )
	{
		con_error( "LodePNG Error when trying to save %s: %s", path, lodepng_error_text( iError ) );
		return; // can't continue
	}

	bool bSuccess = false;
	fileSystem::WriteFile( path, imageData.data(), imageData.size(), bSuccess );

	if ( !bSuccess )
	{
		con_error( "Failed to save texture %s", path );
	}
}

void materialSystem::Init() {}

CTexture *materialSystem::LoadTexture( const char *path )
{
	char *fp = new char[strlen( path ) + 1 + 18];
	fp		 = strcpy( fp, "/assets/textures/" );
	fp		 = strcat( fp, path );

	for ( CTexture *t : loadedTextures )
	{
		// Filepath may be nullptr if it was a texture created from image data
		if ( t->m_cFilePath != nullptr && strcmp( t->m_cFilePath, fp ) == 0 )
		{
			delete[] fp;
			return t;
		}
	}

	CTexture *tex = new CTexture( fp );
	loadedTextures.push_back( tex );

	delete[] fp;
	return loadedTextures.back();
}

CTexture *materialSystem::LoadTexture( std::vector<unsigned char> imageData, unsigned int width, unsigned int height )
{
	CTexture *tex = new CTexture( imageData, width, height );
	loadedTextures.push_back( tex );

	return loadedTextures.back();
}

void materialSystem::ReloadTextures()
{
	for ( CTexture *t : loadedTextures )
		t->Load();
}

void materialSystem::UnInit()
{
	// Unload textures
	for ( CTexture *t : loadedTextures )
		delete t;
}
