// Baob's Awful Immediate-mode GUI
// Baig for short.
//
// The whole GUI is grid-based.
// Sizes are represented in grid units, positions are too.
// 0, 0 is the top-left corner.
//
// 

// TODO: Abstract the OpenGL calls away from the GUI.
#include <glad/glad.h>

#include "gui.hpp"

#include "utility/tomlcpp.hpp"

#include "shared/logging.hpp"
#include "shared/filesystem.hpp"
#include "shared/colours.hpp"

#include "sound/soundmanager.hpp"

#include "utility/utfz.h"
#include "437.hpp"

#include <algorithm>

// How many grid units are there in the screen at all times
#define GUI_GRID_X 54
// Size in pixels of the font we expect
#define EXPECTED_FONT_SIZE 8

CGui::CGui(Vector3f screenDimensions)
{
	// The OpenGL stuff
	{
		glGenVertexArrays( 1, &m_iVao );
		glGenBuffers( 1, &m_iVbo );

		glBindVertexArray( m_iVao );

		glBindBuffer( GL_ARRAY_BUFFER, m_iVbo );
		glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * 6 * 8, NULL, GL_DYNAMIC_DRAW );

		// Position
		glVertexAttribPointer( 0, 3, GL_FLOAT, false, 9 * sizeof( float ), (void *)0 );
		glEnableVertexAttribArray( 0 );
		// texture coordinate
		glVertexAttribPointer( 1, 2, GL_FLOAT, false, 9 * sizeof( float ), (void *)offsetof( CGui::GuiVert, u ) );
		glEnableVertexAttribArray( 1 );
		// colour
		glVertexAttribPointer( 2, 4, GL_FLOAT, false, 9 * sizeof( float ), (void *)offsetof( CGui::GuiVert, r ) );
		glEnableVertexAttribArray( 2 );

		glBindVertexArray( 0 );
	}


	// Setup font atlas
	// Load font definition TOML

	bool bSuccess = false;
	int64_t iFileSize = 0;
	const char* file = (char*)fileSystem::LoadFile("lang/font.toml", iFileSize, bSuccess);

	toml::Result l = toml::parse(bSuccess ? file : "\0");

	// Font definition is as such:
	// [font]
	// textures = ["font.png", "font_b.png"]
	// [font.chars]
	// [tex] = [list of chars]

	// Each font texture is 16x16 tiles, which may be of different sizes.
	// Sizes of characters are inferred by the resolution of each texture.
	// The height of each character is the texture height / 16
	// Width is the furthest pixel with a non-zero alpha value + 1

	// TODO: any of the above!

	con_info( "Processing font widths" );
	m_pFontTex = materialSystem::LoadTexture("font.png");
	int resX = m_pFontTex->m_iWidth / 16;
	int resY = m_pFontTex->m_iHeight / 16;
	for ( uchar_t c = 0; c < 254; c++ )
	{
		if ( c == ' ' )
		{
			m_charWidths[' '] = 1.0;
			continue;
		}

		// we assume the width to be a power of 16 and height to be a power of 16
		int px = ( c % 16 ) * resX;
		int py = ( c / 16 ) * resY;

		int width = 0;
		for ( int y = 0; y < resY; y++ )
		{
			for ( int x = 0; x < resX; x++ )
			{
				// https://github.com/lvandeve/lodepng/blob/master/examples/example_sdl.cpp#L67, amusingly.
				int idx = 4 * ( py + y ) * m_pFontTex->m_iWidth + 4 * ( px + x ) + 3;
				if ( m_pFontTex->m_imageData[idx] && x + 1 >= width )
				{
					width = x + 1;
				}
			}
		}

		m_charWidths[c] = (float)width / (float)resX;
	}

	m_charWidths[255] = 1.0f;

	// Load textures
	m_pButtonTex = materialSystem::LoadTexture("button.png");

	// Load Shader
	m_pShader = shaderSystem::LoadShader("text.vert", "text.frag");

	m_vScreenDimensions.x = screenDimensions.x;
	m_vScreenDimensions.y = screenDimensions.y;
	m_vScreenCentre = m_vScreenDimensions / 2.0f;
	m_iGUIUnitSize = screenDimensions.x / GUI_GRID_X;
}

CGui::~CGui()
{
	// OpenGl
	{
		glDeleteBuffers( 1, &m_iVao );
		glDeleteVertexArrays( 1, &m_iVbo );
	}
}

void CGui::Resize( Vector3f screenDimensions )
{
	m_vScreenDimensions.x = screenDimensions.x;
	m_vScreenDimensions.y = screenDimensions.y;
	m_vScreenCentre = m_vScreenDimensions / 2.0f;
	m_iGUIUnitSize = screenDimensions.x / GUI_GRID_X;
}

void CGui::Update()
{
	if ( m_iMouseState == IN_NO_MOUSE )
		m_iActiveItem = 0;

	m_vMousePos.x = m_pInputManager->m_vMousePos.x;
	m_vMousePos.y = m_pInputManager->m_vMousePos.y;
	
	m_iMouseState = m_pInputManager->m_iMouseState;
	// else if (m_iActiveItem == 0)
	// 	m_iActiveItem = -1;

	m_iHotItem = 0;

	// Render
	{
		int depthFunc;
		glGetIntegerv( GL_DEPTH_FUNC, &depthFunc );
		glDepthFunc( GL_ALWAYS );

		m_pShader->Use();

		// Text
		glBindVertexArray( m_iVao );
		{
			glBindTexture( GL_TEXTURE_2D, m_pFontTex->m_iId );

			glBindBuffer( GL_ARRAY_BUFFER, m_iVbo );
			glBufferData( GL_ARRAY_BUFFER, m_vertices.size() * sizeof( CGui::GuiVert ), m_vertices.data(),
						  GL_DYNAMIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glDrawArrays( GL_TRIANGLES, 0, m_vertices.size() );

			glBindTexture( GL_TEXTURE_2D, 0 );
			
			// Images
			// TODO: bad bad bad die!
			for ( CGui::GuiImage img : m_images )
			{
				glBindTexture( GL_TEXTURE_2D, img.pTex->m_iId );
				glBindBuffer( GL_ARRAY_BUFFER, m_iVbo );
				glBufferData( GL_ARRAY_BUFFER, img.verts.size() * sizeof( CGui::GuiVert ), img.verts.data(),
							  GL_DYNAMIC_DRAW );
				glBindBuffer( GL_ARRAY_BUFFER, 0 );
				glDrawArrays( GL_TRIANGLES, 0, img.verts.size() );
				glBindTexture( GL_TEXTURE_2D, 0 );
			}
		}

		glBindVertexArray( 0 );

		glDepthFunc( depthFunc );
	}

	// Clear our vertices
	m_vertices.clear();
	m_images.clear();
}

bool CGui::RegionHit( Vector3f pos, Vector3f size )
{
	pos.y = m_vScreenDimensions.y - pos.y;

	if ( m_vMousePos.x <= pos.x || m_vMousePos.y >= pos.y || m_vMousePos.x >= pos.x + size.x ||
		 m_vMousePos.y <= pos.y - size.y )
		return false;
	return true;
}

std::vector<CGui::GuiVert> CGui::GetRect( Vector3f pos, Vector3f size, Vector4f uv, CColour colour )
{
	Vector4f c = colour;
	return {
		{ pos.x + size.x, pos.y, pos.z, uv.z, uv.w, c.x, c.y, c.z, c.w },
		{ pos.x, pos.y, pos.z, uv.x, uv.w, c.x, c.y, c.z, c.w },
		{ pos.x, pos.y + size.y, pos.z, uv.x, uv.y, c.x, c.y, c.z, c.w },

		{ pos.x + size.x, pos.y + size.y, pos.z, uv.z, uv.y, c.x, c.y, c.z, c.w },
		{ pos.x + size.x, pos.y, pos.z, uv.z, uv.w, c.x, c.y, c.z, c.w },
		{ pos.x, pos.y + size.y, pos.z, uv.x, uv.y, c.x, c.y, c.z, c.w }
	};
}

Vector3f CGui::GetInScreen( Vector3f pos )
{
	pos = pos * m_iGUIUnitSize;

	// Negative values mean start from the opposite side
	if ( pos.x < 0 )
		pos.x = m_vScreenDimensions.x + pos.x;
	if ( pos.y < 0 )
		pos.y = m_vScreenDimensions.y + pos.y;

	return pos;
}

void CGui::_Image( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint )
{
	GuiImage img;
	img.pTex = pTex;
	img.verts = GetRect( pos, size, Vector4f( 0, 0, 1, 1 ), tint );
	m_images.push_back( img );
}

//////////////////////////////////////////////////////////////////////////
// Elements
//////////////////////////////////////////////////////////////////////////

void CGui::Image( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint )
{
	pos = GetInScreen( pos );
	size = size * m_iGUIUnitSize;

	_Image( pos, size, pTex, tint );
}

bool CGui::Button( GuiID id, Vector3f position, Vector3f size, CTexture *pTexture )
{
	position = GetInScreen( position );
	size *= m_iGUIUnitSize;
	
	if ( pTexture == nullptr )
		pTexture = m_pButtonTex;

	int returnCode = 0;

	CColour color = CColour( 255, 255, 255 );

	// Check & set State
	if ( RegionHit( position, size ) )
	{
		m_iHotItem = id;
		color	   = CColour( 191, 191, 255 );
		if ( m_iActiveItem == 0 && ( m_iMouseState == IN_LEFT_MOUSE ) )
		{
			m_iActiveItem = id;

			soundSystem::PlaySoundEvent( "ui.click", Vector3f( 0, 0, 0 ) );
		}
	}

	if ( m_iMouseState == IN_NO_MOUSE && m_iHotItem == id && m_iActiveItem == id )
	{
		returnCode = 1;
		color	   = CColour( 63, 63, 63 );
	}

	// Draw
	{
		_Image( position, size, pTexture, color );
	}

	return returnCode;
}

void CGui::Label( const char* text, Vector3f position, float scale, CColour colour )
{
	scale *= 2.0f;
	position = GetInScreen( position );

	bool bRandomise = false;

	float onePixel = scale * (2.0f / 16.0f) * (float)m_iGUIUnitSize;

	// Render
	// OpenGl
	{
		int i = 0;

		int c;

		while ( utfz::next( text, c ) )
		{
			// Find the index of the character in the font
			int j = std::distance( CP437UNICODE, std::find( CP437UNICODE, CP437UNICODE + 256, c ) );

			if ( j == 0 || j >= 256 )
				j = 255;
			
			// Get the character's UV coordinates
			Vector4f uv;
			float x, y;
			x = ( j % 16 ) * EXPECTED_FONT_SIZE;
			y = ( j / 16 ) * EXPECTED_FONT_SIZE;

			uv.x = x / ( 16.0f * EXPECTED_FONT_SIZE );
			uv.y = y / ( 16.0f * EXPECTED_FONT_SIZE );
			uv.z = ( x + EXPECTED_FONT_SIZE ) / ( 16.0f * EXPECTED_FONT_SIZE );
			uv.w = ( y + EXPECTED_FONT_SIZE ) / ( 16.0f * EXPECTED_FONT_SIZE );

			// Render the character
			// Shadow first
			std::vector<GuiVert> vertices = GetRect( position + Vector3f( i, 0, 0 ) + Vector3f{ onePixel, -onePixel },
													 Vector3f( m_iGUIUnitSize * scale, m_iGUIUnitSize * scale ),
													 uv, colour / CColour(2, 2, 2, 1) );
			m_vertices.insert( m_vertices.end(), vertices.begin(), vertices.end() );
			// Text character
			vertices = GetRect( position + Vector3f( i, 0, 0 ),
													 Vector3f( m_iGUIUnitSize * scale, m_iGUIUnitSize * scale ),
													 uv, colour );
			m_vertices.insert( m_vertices.end(), vertices.begin(), vertices.end() );

			i += m_charWidths[j] * m_iGUIUnitSize * scale;
			i += onePixel;
		}
	}
}