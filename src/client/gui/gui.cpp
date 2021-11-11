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
#define GUI_GRID_X 53
// Size in pixels of the font we expect
#define EXPECTED_FONT_SIZE 8


#define BUTTON_EDGE_RADIUS 3

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
	m_pTextEditTex = materialSystem::LoadTexture("textinput.png");
	m_pSliderTex = materialSystem::LoadTexture("slider.png");
	m_pSliderThumbTex = materialSystem::LoadTexture("thumb.png");
	m_pCheckedBoxTex = materialSystem::LoadTexture("checkbox-checked.png");
	m_pUnCheckedBoxTex = materialSystem::LoadTexture("checkbox-unchecked.png");

	// Load the useful textures
	m_pGuiBGTex = materialSystem::LoadTexture("guibg.png");
	m_pGuiTitleTex = materialSystem::LoadTexture("title.png");

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

	m_pInputManager->m_iCursor = m_iHotItem != 0 ? CURSOR_HAND : CURSOR_ARROW;

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
			// We draw Images first, then Text
			// TODO: ideally we'd not care, but I can't figure out how to make the Z coordinate work in ortho mode.
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
			glBindTexture( GL_TEXTURE_2D, m_pFontTex->m_iId );

			glBindBuffer( GL_ARRAY_BUFFER, m_iVbo );
			glBufferData( GL_ARRAY_BUFFER, m_vertices.size() * sizeof( CGui::GuiVert ), m_vertices.data(),
						  GL_DYNAMIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glDrawArrays( GL_TRIANGLES, 0, m_vertices.size() );

			glBindTexture( GL_TEXTURE_2D, 0 );
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
		{ pos.x, pos.y - size.y, pos.z, uv.x, uv.y, c.x, c.y, c.z, c.w },

		{ pos.x + size.x, pos.y - size.y, pos.z, uv.z, uv.y, c.x, c.y, c.z, c.w },
		{ pos.x + size.x, pos.y, pos.z, uv.z, uv.w, c.x, c.y, c.z, c.w },
		{ pos.x, pos.y - size.y, pos.z, uv.x, uv.y, c.x, c.y, c.z, c.w }
	};
}

Vector3f CGui::GetInScreen( Vector3f pos )
{
	pos *= m_iGUIUnitSize;

	// Negative values get flipped to the other side of the screen
	if ( pos.x < 0 ) {
		pos.x += m_iGUIUnitSize; // if it's -1 then it's 0
		pos.x = m_vScreenDimensions.x + pos.x;
	}
	if ( pos.y < 0 ) {
		pos.y += m_iGUIUnitSize; // if it's -1 then it's 0
		pos.y = m_vScreenDimensions.y + pos.y;
	}

	return pos;
}

void CGui::_Image( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint, Vector4f uv )
{
	GuiImage img;
	img.pTex = pTex;
	img.verts = GetRect( pos, size, uv, tint );
	m_images.push_back( img );
}

void CGui::_9PatchRect( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint, float borderSize )
{
	// Creats image with 9-patch
	float v = m_iGUIUnitSize * 0.5f;

	// Corners first
	_Image( { pos.x, pos.y - size.y + v }, { v, v }, pTex, tint, { 0, 0, 1/borderSize, 1/borderSize } );
	_Image( { pos.x + size.x - v, pos.y - size.y + v }, { v, v }, pTex, tint, { 1 - 1/borderSize, 0, 1, 1/borderSize } );
	_Image( { pos.x, pos.y }, { v, v }, pTex, tint, { 0, 1 - 1/borderSize, 1/borderSize, 1 } );
	_Image( { pos.x + size.x - v, pos.y }, { v, v }, pTex, tint, { 1 - 1/borderSize, 1 - 1/borderSize, 1, 1 } );

	// Edges
	_Image( { pos.x + v, pos.y }, { size.x - v * 2, v }, pTex, tint, { 1/borderSize, 1 - 1/borderSize, 1 - 1/borderSize, 1 } );
	_Image( { pos.x + v, pos.y - size.y + v }, { size.x - v * 2, v }, pTex, tint, { 1/borderSize, 0, 1 - 1/borderSize, 1/borderSize } );
	_Image( { pos.x, pos.y - v }, { v, size.y - v * 2 }, pTex, tint, { 0, 1/borderSize, 1/borderSize, 1 - 1/borderSize } );
	_Image( { pos.x + size.x - v, pos.y - v }, { v, size.y - v * 2 }, pTex, tint, { 1 - 1/borderSize, 1/borderSize, 1, 1 - 1/borderSize } );

	// Center
	_Image( { pos.x + v, pos.y - v }, { size.x - v * 2, size.y - v * 2 }, pTex, tint, { 1/borderSize, 1/borderSize, 1 - 1/borderSize, 1 - 1/borderSize } );
}

float CGui::_TextLength(const char *text, float scale)
{
	float total = 0.0f;
	float onePixel = scale * (2.0f / 16.0f) * (float)m_iGUIUnitSize;

	int c;

	while ( utfz::next( text, c ) )
	{
		// Find the index of the character in the font
		int j = std::distance( CP437UNICODE, std::find( CP437UNICODE, CP437UNICODE + 256, c ) );

		if ( j == 0 || j >= 256 )
			j = 255;
		
		// Add the width of the character to the total
		total += m_charWidths[j] * m_iGUIUnitSize * scale;
		total += onePixel;
	}

	return total;
}

//////////////////////////////////////////////////////////////////////////
// Base Elements
//////////////////////////////////////////////////////////////////////////

void CGui::Image( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint )
{
	pos = GetInScreen( pos );
	size = size * m_iGUIUnitSize;

	_Image( pos, size, pTex, tint );
}

// Creates an image that repeats its' texture to fill the entire area
// Without stretching the texture
void CGui::ImageRepeating( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint )
{
	pos = GetInScreen( pos );
	size = size * m_iGUIUnitSize;

	// UV coordinates, one pixel of the texture is a 16th of the gui unit
	// So that a 16x16 texture will be two gui units


	// we need to take into account the size of the texture, the size of the area we want to fill, and the size of the gui unit
	// to get the correct UV coordinates
	Vector4f uv = {
		0, 0,
		size.x / (float)m_iGUIUnitSize / (float)pTex->m_iWidth * 8.0f, size.y / (float)m_iGUIUnitSize / (float)pTex->m_iHeight * 8.0f
	};


	_Image( pos, size, pTex, tint, uv );
}

void CGui::ImageCentered( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint )
{
	pos = GetInScreen( pos );
	size = size * m_iGUIUnitSize;

	pos.x -= size.x * 0.5f;
	pos.y += size.y * 0.5f;

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
		_9PatchRect( position, size, pTexture, color, BUTTON_EDGE_RADIUS );
	}

	return returnCode;
}

void CGui::Label( const char* text, Vector3f position, float scale, CColour colour, TextAlignment alignment )
{
	position = GetInScreen( position );

	switch ( alignment)
	{
		case TEXTALIGN_LEFT:
			break;
		case TEXTALIGN_CENTER:
			position.x -= (_TextLength( text, scale ) * 0.5f) / (float)m_iGUIUnitSize;
			break;
		case TEXTALIGN_RIGHT:
			position.x -= (_TextLength( text, scale )) / (float)m_iGUIUnitSize;
			break;
	}

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
			std::vector<GuiVert> vertices = GetRect( position + Vector3f( i, 0, 0 ) + Vector3f{ onePixel, onePixel },
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

const char *CGui::TextInput( int id, Vector3f vPosition )
{

	std::string text = m_textBuffers[id];

	std::string dispText = (m_iTick / 8) % 2 == 0 ? text + "_" : text;
	Label( dispText.c_str(), vPosition - Vector3f(0, 1), 1.0f, CColour( 255, 255, 255 ) );

	if ( m_pInputManager->m_cTypeKey != nullptr )
	{
		text += m_pInputManager->m_cTypeKey;
	}

	if ( m_pInputManager->m_clipboard.size() )
		text += m_pInputManager->m_clipboard;

	if ( m_pInputManager->m_bKeyboardState[KBD_BACKSPACE] && !m_pInputManager->m_bOldKeyboardState[KBD_BACKSPACE] )
	{
		// TODO: Figure out the length in bytes of the last character, and then remove exactly that many bytes from the end
		if ( text.length() )
		{
			if ( text.length() >= 2 )
			{
				std::string g = text.substr( text.length() - 2 );

				int a = utfz::decode( g.c_str() );
				if ( a > 127 && a != utfz::replace )
				{
					// HACK: Remove two :trollface:
					text.pop_back();
				}
			}

			text.pop_back();
		}
	}

	m_textBuffers[id] = text;

	if ( m_pInputManager->m_bKeyboardState[KBD_RETURN] && !m_pInputManager->m_bOldKeyboardState[KBD_RETURN] )
		return m_textBuffers[id].c_str();
	else
		return nullptr;
}

bool CGui::Slider( int id, Vector3f pos, Vector3f size, int max, int &value )
{
	pos	 = GetInScreen( pos );
	float sY = size.y;
	size = size * m_iGUIUnitSize;

	_9PatchRect( pos, size, m_pSliderTex, {255, 255, 255}, BUTTON_EDGE_RADIUS );

	// The ypos is the percentage of the slider that the value is at
	float ypos = (float)value / (float)max;
	ypos *= (sY - 2.0f); // The thumb is 2 units thick, so we need to subtract that from the slider height

	_Image({pos.x, pos.y - ypos * m_iGUIUnitSize}, {2 * (float)m_iGUIUnitSize, 2 * (float)m_iGUIUnitSize}, m_pSliderThumbTex, {255, 255, 255});

	if ( RegionHit( pos, size ) )
	{
		m_iHotItem = id;

		if ( m_iActiveItem == 0 && ( m_iMouseState == IN_LEFT_MOUSE ) )
		{
			m_iActiveItem = id;
		}
	}

	if ( m_iActiveItem == id )
	{
		// find the relative y position of the mouse
		// And the percentage of our y size that is
		float y = (float)m_pInputManager->m_vMousePos.y - (pos.y - size.y);
		float p = y / (size.y - (2.0f * m_iGUIUnitSize));

		p = 1.0f - p;

		// Clamp it to the slider
		if ( p < 0 )
			p = 0;
		else if ( p > 1 )
			p = 1;
		
		int val = floor(p * max);

		if ( val != value )
		{
			// if ( m_iTick % 2 == 0 )
			// 	soundSystem::PlaySoundEvent( "ui.tick", Vector3f( 0, 0, 0 ) );

			value = val;
			return true;
		}
	}

	return false;
}

bool CGui::HorzSlider( int id, Vector3f pos, Vector3f size, int max, int &value )
{
	pos	 = GetInScreen( pos );
	float sX = size.x;
	size = size * m_iGUIUnitSize;

	_9PatchRect( pos, size, m_pSliderTex, {255, 255, 255}, BUTTON_EDGE_RADIUS );

	// The ypos is the percentage of the slider that the value is at
	float xpos = (float)value / (float)max;
	xpos *= (sX - 2.0f); // The thumb is 2 units thick, so we need to subtract that from the slider height

	_Image({pos.x + xpos * m_iGUIUnitSize, pos.y}, {2 * (float)m_iGUIUnitSize, 2 * (float)m_iGUIUnitSize}, m_pSliderThumbTex, {255, 255, 255});

	if ( RegionHit( pos, size ) )
	{
		m_iHotItem = id;

		if ( m_iActiveItem == 0 && ( m_iMouseState == IN_LEFT_MOUSE ) )
		{
			m_iActiveItem = id;
		}
	}

	if ( m_iActiveItem == id )
	{
		// find the relative x position of the mouse
		// And the percentage of our x size that is
		float x = (float)m_pInputManager->m_vMousePos.x - pos.x;
		float p = x / (size.x - (2.0f * m_iGUIUnitSize));

		// Clamp it to the slider
		if ( p < 0 )
			p = 0;
		else if ( p > 1 )
			p = 1;
		
		int val = floor(p * max);

		if ( val != value )
		{
			// if ( m_iTick % 2 == 0 )
			// 	soundSystem::PlaySoundEvent( "ui.tick", Vector3f( 0, 0, 0 ) );

			value = val;
			return true;
		}
	}

	return false;
}

bool CGui::CheckBox( int id, Vector3f pos, bool &value )
{
	pos	 = GetInScreen( pos );
	Vector3f size = Vector3f{2,2} * m_iGUIUnitSize;

	CTexture *tex = m_pUnCheckedBoxTex;
	if ( value )
		tex = m_pCheckedBoxTex;

	_Image( pos, size, tex, {255, 255, 255} );

	if ( RegionHit( pos, size ) )
	{
		m_iHotItem = id;

		if ( m_iActiveItem == 0 && ( m_iMouseState == IN_LEFT_MOUSE ) )
		{
			m_iActiveItem = id;

			value = !value;
			soundSystem::PlaySoundEvent( value ? "ui.check" : "ui.uncheck", Vector3f( 0, 0, 0 ) );
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// Composite Elements
//////////////////////////////////////////////////////////////////////////

bool CGui::LabelButton( GuiID id, const char* text, Vector3f position, Vector3f minSize, TextAlignment alignment )
{
	bool returnValue = false;

	// Get the size of the text
	float textLength = _TextLength( text, 1.0f ) / (float)m_iGUIUnitSize;

	float textOffset = 0.5f;

	// Get the size of the button
	Vector3f size = minSize;
	size.x = std::max( size.x, textLength );

	// Now draw the button
	returnValue = Button( id, position, size, m_pButtonTex );

	// Now draw the text
	// (taking into account the alignment and BUTTON_EDGE_RADIUS)
	// TODO: These are buggy, but they look fine on 1920x1080 so I don't care lol
	switch ( alignment )
	{
		case TEXTALIGN_LEFT:
			// Draw it glued to the left border of the button
			Label( text, position + Vector3f( textOffset, -textOffset ), 1.0f, CColour( 255, 255, 255 ), alignment );
		break;
		case TEXTALIGN_CENTER:
			// Draw it in the middle of the button
			Label( text, position + Vector3f( ( size.x / 2.0f ) + textOffset - ( textLength / 2.0f ), -textOffset ), 1.0f, CColour( 255, 255, 255 ), alignment );
		break;
		case TEXTALIGN_RIGHT:
			// Draw it glued to the right border of the button
			Label( text, position + Vector3f( size.x - textLength, -textOffset ), 1.0f, CColour( 255, 255, 255 ), alignment );
		break;
	}

	return returnValue;
}

// Selectable == not always typeable
const char *CGui::SelectableTextInput( int id, Vector3f pos, Vector3f size )
{
	pos	 = GetInScreen( pos );
	size = size * m_iGUIUnitSize;

	CColour color( 255, 255, 255 );
	CColour textColour( 255, 255, 255 );

	if ( RegionHit( pos, size ) )
	{
		m_iHotItem = id;
		color	   = CColour( 191, 191, 255);
		textColour  = CColour( 255, 255, 191 );

		if ( m_iActiveItem == 0 && ( m_iMouseState == IN_LEFT_MOUSE ) )
		{
			m_iActiveItem	= id;
			m_iKeyboardItem = id;
		}
	}

	if ( m_iKeyboardItem != id )
		Label( m_textBuffers[id].c_str(), ( pos / (float)m_iGUIUnitSize ) + Vector3f( 0.5, 0.5 ), 1.0f, textColour );
	else
	{
		TextInput( id, ( pos / (float)m_iGUIUnitSize )  + Vector3f( 0.5, 0.5 ) );
	}

	// Draw the box
	_9PatchRect( pos, size, m_pTextEditTex, color, BUTTON_EDGE_RADIUS );

	return nullptr;
}