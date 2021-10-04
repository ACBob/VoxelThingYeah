// An ImGui
// TODO: Abstract OpenGL away from this, making it renderer agnostic
//       (Store like a mesh, have some kind of renderer)

#include "gui.hpp"

#include <glad/glad.h>

#include "437.hpp"

#include "utfz.h"

#include <algorithm>

// The whole coordinate system is based on these GUIUUNITs.
// When we start to dynamically rescale the GUI based on resolution this will come in handy. for now, 16px.
#define GUIUNIT m_iGuiUnit

#define TEXTINTEXWIDTH 8
#define TEXTINTEXHEIGHT 8

#define TEXTWIDTH GUIUNIT
#define TEXTHEIGHT GUIUNIT

#define LETTERSPACING 8 / GUIUNIT

// I am not supporting weird configurations
#define TEXTTILES 16

#include "shared/logging.hpp"

// TODO: global?
float fontWidths[256];

int CGui::GetTextLength( const char *msg )
{
	int l = 0;

	int i = 0;
	while ( msg[i] != NULL )
	{
		l += fontWidths[std::distance( CP437UNICODE, std::find( CP437UNICODE, CP437UNICODE + 256, msg[i] ) )] *
			 TEXTWIDTH;
		l += 2.0f / 16.0f * (float)GUIUNIT;

		i++;
	}

	return l;
}

CGui::CGui( int screenW, int screenH )
	: m_iMouseState( IN_NO_MOUSE ), m_iActiveItem( 0 ), m_iHotItem( 0 ), m_iKeyboardItem( 0 )
{
	// OpenGl
	{
		glGenVertexArrays( 1, &m_iVao );
		glGenBuffers( 1, &m_iVbo );

		glBindVertexArray( m_iVao );

		glBindBuffer( GL_ARRAY_BUFFER, m_iVbo );
		glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * 6 * 8, NULL, GL_DYNAMIC_DRAW );

		// + sizeof(Texture*) so we skip the little internal thing at the end

		// Position
		glVertexAttribPointer( 0, 3, GL_FLOAT, false, 8 * sizeof( float ), (void *)0 );
		glEnableVertexAttribArray( 0 );
		// texture coordinate
		glVertexAttribPointer( 1, 2, GL_FLOAT, false, 8 * sizeof( float ), (void *)offsetof( CGui::Vertex, u ) );
		glEnableVertexAttribArray( 1 );
		// colour
		glVertexAttribPointer( 2, 3, GL_FLOAT, false, 8 * sizeof( float ), (void *)offsetof( CGui::Vertex, r ) );
		glEnableVertexAttribArray( 2 );

		glBindVertexArray( 0 );
	}

	// Setup some stff that'll be used by most things using the GUI
	m_pTextTex		= materialSystem::LoadTexture( "font.png" );
	m_pTextShader	= shaderSystem::LoadShader( "shaders/text.vert", "shaders/text.frag" );
	m_pButtonTex	= materialSystem::LoadTexture( "button.png" );
	m_pTextInpTex	= materialSystem::LoadTexture( "textinput.png" );
	m_pBGTex		= materialSystem::LoadTexture( "guibg.png" );
	m_pCrosshairTex = materialSystem::LoadTexture( "crosshair.png" );
	m_pInventoryTex = materialSystem::LoadTexture( "inventory.png" );
	m_pLogoTex		= materialSystem::LoadTexture( "title.png" );
	m_pSliderTex	= materialSystem::LoadTexture( "slider.png" );
	m_pThumbTex		= materialSystem::LoadTexture( "thumb.png" );
	m_pCheckedTex	= materialSystem::LoadTexture( "checkbox-checked.png" );
	m_pUncheckedTex = materialSystem::LoadTexture( "checkbox-unchecked.png" );

	m_textBuffers = {};

	Resize( screenW, screenH );

	con_info( "Processing font widths" );
	int resX = m_pTextTex->m_iWidth / 16;
	int resY = m_pTextTex->m_iHeight / 16;
	for ( uchar_t c = 0; c < 255; c++ )
	{
		if ( c == ' ' )
		{
			fontWidths[' '] = 1.0;
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
				int idx = 4 * ( py + y ) * m_pTextTex->m_iWidth + 4 * ( px + x ) + 3;
				if ( m_pTextTex->m_imageData[idx] && x + 1 >= width )
				{
					width = x + 1;
				}
			}
		}

		fontWidths[c] = (float)width / (float)resX;
	}

	fontWidths[255] = 1.0f;

	m_iGuiUnit = 16;
}

void CGui::Resize( int x, int y )
{
	m_iGuiUnit			= x / 53;
	m_vScreenCentre		= CVector( ( x * 0.5 ) / GUIUNIT, ( y * 0.5 ) / GUIUNIT );
	m_vScreenDimensions = CVector( x, y );
}

CGui::~CGui()
{
	// OpenGl
	{
		glDeleteBuffers( 1, &m_iVao );
		glDeleteVertexArrays( 1, &m_iVbo );
	}
}

void CGui::Update()
{
	if ( m_iMouseState == IN_NO_MOUSE )
		m_iActiveItem = 0;

	m_vMousePos	  = m_pInputMan->m_vMousePos;
	m_iMouseState = m_pInputMan->m_iMouseState;
	// else if (m_iActiveItem == 0)
	// 	m_iActiveItem = -1;

	m_iHotItem = 0;

	// Render
	{
		int depthFunc;
		glGetIntegerv( GL_DEPTH_FUNC, &depthFunc );
		glDepthFunc( GL_ALWAYS );

		m_pTextShader->Use();

		// Images
		glBindVertexArray( m_iVao );
		{
			for ( CGui::_Image img : m_images )
			{
				glBindTexture( GL_TEXTURE_2D, img.m_pTex->m_iId );
				glBindBuffer( GL_ARRAY_BUFFER, m_iVbo );
				glBufferData( GL_ARRAY_BUFFER, img.m_vertices.size() * sizeof( CGui::Vertex ), img.m_vertices.data(),
							  GL_DYNAMIC_DRAW );
				glBindBuffer( GL_ARRAY_BUFFER, 0 );
				glDrawArrays( GL_TRIANGLES, 0, img.m_vertices.size() );
				glBindTexture( GL_TEXTURE_2D, 0 );
			}
		}
		glBindVertexArray( 0 );
		// Text
		glBindVertexArray( m_iVao );
		{
			glBindTexture( GL_TEXTURE_2D, m_pTextTex->m_iId );

			glBindBuffer( GL_ARRAY_BUFFER, m_iVbo );
			glBufferData( GL_ARRAY_BUFFER, m_textVertiecs.size() * sizeof( CGui::Vertex ), m_textVertiecs.data(),
						  GL_DYNAMIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glDrawArrays( GL_TRIANGLES, 0, m_textVertiecs.size() );

			glBindTexture( GL_TEXTURE_2D, 0 );
		}
		glBindVertexArray( 0 );

		glDepthFunc( depthFunc );
	}

	// Clear our vertices
	m_textVertiecs.clear();
	// Clear our images
	m_images.clear();
}

std::vector<CGui::Vertex> CGui::GetQuad( CVector pos, CVector size, Colour color, CVector uStart, CVector uEnd )
{
	return {
		{ pos.x + size.x, pos.y, 0, uEnd.x, uEnd.y, color.x, color.y, color.z },
		{ pos.x, pos.y, 0, uStart.x, uEnd.y, color.x, color.y, color.z },
		{ pos.x, pos.y + size.y, 0, uStart.x, uStart.y, color.x, color.y, color.z },

		{ pos.x + size.x, pos.y + size.y, 0, uEnd.x, uStart.y, color.x, color.y, color.z },
		{ pos.x + size.x, pos.y, 0, uEnd.x, uEnd.y, color.x, color.y, color.z },
		{ pos.x, pos.y + size.y, 0, uStart.x, uStart.y, color.x, color.y, color.z },
	};
}
std::vector<CGui::Vertex> CGui::GetCharQuad( const int c, CVector pos, CVector size, Colour color )
{

	float x, y;
	x = ( c % TEXTTILES ) * TEXTINTEXWIDTH;
	y = ( c / TEXTTILES ) * TEXTINTEXHEIGHT;

	CVector uStart = {
		x / ( 16.0f * TEXTINTEXWIDTH ),
		y / ( 16.0f * TEXTINTEXHEIGHT ),
	};
	CVector uEnd = {
		( x + TEXTINTEXWIDTH ) / ( 16.0f * TEXTINTEXWIDTH ),
		( y + TEXTINTEXHEIGHT ) / ( 16.0f * TEXTINTEXHEIGHT ),
	};

	return GetQuad( pos, size, color, uStart, uEnd );
}

void CGui::ClearBuffers()
{
	m_textBuffers.clear();
	m_iActiveItem = m_iHotItem = m_iKeyboardItem = m_iMouseState = 0;
}

void CGui::SetTextBuffer( int id, const char *text ) { m_textBuffers[id] = text; }

const char *CGui::GetTextBuffer( int id ) { return m_textBuffers[id].c_str(); }

bool CGui::RegionHit( CVector pos, CVector size )
{
	pos.y = m_vScreenDimensions.y - pos.y;

	if ( m_vMousePos.x <= pos.x || m_vMousePos.y >= pos.y || m_vMousePos.x >= pos.x + size.x ||
		 m_vMousePos.y <= pos.y - size.y )
		return false;
	return true;
}

CVector CGui::GetInScreen( CVector pos )
{
	pos = pos * GUIUNIT;

	if ( pos.x < 0 )
		pos.x += m_vScreenDimensions.x;
	if ( pos.y < 0 )
		pos.y += m_vScreenDimensions.y;

	return pos;
}

int CGui::Button( int id, CVector pos, CVector size, CVector origin, CTexture *tex, bool hide )
{
	pos	 = GetInScreen( pos );
	size = size * GUIUNIT;
	pos	 = pos - ( size * origin );
	if ( tex == nullptr )
		tex = m_pButtonTex;

	int returnCode = 0;

	Colour color = Colour( 1, 1, 1 );

	// Check & set State
	if ( RegionHit( pos, size ) )
	{
		m_iHotItem = id;
		color	   = Colour( 0.75, 0.75, 1 );
		if ( m_iActiveItem == 0 && ( m_iMouseState == IN_LEFT_MOUSE ) )
		{
			m_iActiveItem = id;
		}
	}

	if ( m_iMouseState == IN_NO_MOUSE && m_iHotItem == id && m_iActiveItem == id )
	{
		returnCode = 1;
		color	   = Colour( 0.25, 0.25, 0.25 );
	}

	CVector p = pos / GUIUNIT;
	CVector s = size / GUIUNIT;

	if ( !hide )
	{
		Image9Rect( tex, p, s, color );
	}

	return returnCode;
}

int CGui::AtlasButton( int id, CTexture *tex, Atlas atlas, float atlasDivisions, CVector pos, CVector size,
					   CVector origin )
{
	int b = Button( id, pos, size, origin, nullptr, true );
	ImageAtlas( tex, atlas, atlasDivisions, pos, size, origin );
	return b;
}

int CGui::LabelButton( int id, const char *msg, CVector pos, CVector origin, CVector padding, CVector minsize )
{
	// Get size, fixed position
	CVector size = ( CVector( GetTextLength( msg ), TEXTHEIGHT ) + padding * GUIUNIT );

	size.x = fmaxf( size.x, minsize.x * GUIUNIT );
	size.y = fmaxf( size.y, minsize.y * GUIUNIT );

	// Render and get output of button
	int buttonOut = Button( id, pos, size / GUIUNIT, origin );

	pos = pos - ( size / GUIUNIT ) * origin;

	// TODO: it's in the floor
	Label( msg, ( pos + CVector( 0, -0.5 ) + ( size / GUIUNIT ) * origin ), CVector( 1, 1, 1 ), TEXTALIGN_CENTER );
	return buttonOut;
}

void CGui::Label( const char *text, CVector pos, Colour color, TextAlignment textAlign )
{
	pos = GetInScreen( pos );

	if ( textAlign == TEXTALIGN_CENTER )
		pos = pos - CVector( GetTextLength( text ) / 2, 0 );
	else if ( textAlign == TEXTALIGN_RIGHT )
		pos = pos - CVector( GetTextLength( text ), 0 );

	// Render
	// OpenGl
	{
		int i = 0;

		int c;

		while ( utfz::next( text, c ) )
		{
			// Get vertices
			// Shadow
			int j = std::distance( CP437UNICODE, std::find( CP437UNICODE, CP437UNICODE + 256, c ) );

			if ( j == 0 || j >= 256 )
				j = 255;

			std::vector<CGui::Vertex> g =
				GetCharQuad( j, pos - ( 2.0f / 16.0f * (float)GUIUNIT ), CVector( TEXTWIDTH, TEXTHEIGHT ), color / 2 );
			std::copy( g.begin(), g.end(), std::back_inserter( m_textVertiecs ) );

			g = GetCharQuad( j, pos, CVector( TEXTWIDTH, TEXTHEIGHT ), color );
			std::copy( g.begin(), g.end(), std::back_inserter( m_textVertiecs ) );

			pos.x += fontWidths[j] * TEXTWIDTH;
			pos.x += 2.0f / 16.0f * (float)GUIUNIT;
		}
	}
}

void CGui::Image( CTexture *tex, CVector pos, CVector size, CVector origin, Colour tint )
{
	pos	 = GetInScreen( pos );
	size = size * GUIUNIT;

	{
		CGui::_Image img;
		img.m_vertices = GetQuad( pos - ( size * origin ), size, tint );
		img.m_pTex	   = tex;
		m_images.push_back( img );
	}
}

void CGui::ImageAtlas( CTexture *tex, Atlas atlas, float atlasDivisions, CVector pos, CVector size, CVector origin,
					   CVector tint )
{
	pos	 = GetInScreen( pos );
	size = size * GUIUNIT;

	{
		CGui::_Image img;
		img.m_vertices =
			GetQuad( pos - ( size * origin ), size, tint, { atlas.x / atlasDivisions, atlas.y / atlasDivisions },
					 { ( atlas.x + atlas.sizex ) / atlasDivisions, ( atlas.y + atlas.sizey ) / atlasDivisions } );
		img.m_pTex = tex;
		m_images.push_back( img );
	}
}

void CGui::Image9Rect( CTexture *tex, CVector pos, CVector size, Colour color )
{
	// Corners of the 9rect
	ImageAtlas( tex, { 0, 0, 1, 1 }, 3, pos + CVector( 0, size.y - 0.5 ), CVector( 0.5, 0.5 ), CVector( 0, 0 ), color );
	ImageAtlas( tex, { 2, 0, 1, 1 }, 3, pos + CVector( size.x, size.y - 0.5 ), CVector( 0.5, 0.5 ), CVector( 0, 0 ),
				color );
	ImageAtlas( tex, { 0, 2, 1, 1 }, 3, pos, CVector( 0.5, 0.5 ), CVector( 0, 0 ), color );
	ImageAtlas( tex, { 2, 2, 1, 1 }, 3, pos + CVector( size.x, 0 ), CVector( 0.5, 0.5 ), CVector( 0, 0 ), color );

	// Edges
	ImageAtlas( tex, { 1, 0, 1, 1 }, 3, pos + CVector( 0.5, size.y - 0.5 ), CVector( size.x - 0.5, 0.5 ),
				CVector( 0, 0 ), color );
	ImageAtlas( tex, { 1, 2, 1, 1 }, 3, pos + CVector( 0.5, 0 ), CVector( size.x - 0.5, 0.5 ), CVector( 0, 0 ), color );

	ImageAtlas( tex, { 0, 1, 1, 1 }, 3, pos + CVector( 0, 0.5 ), CVector( 0.5, size.y - 1 ), CVector( 0, 0 ), color );
	ImageAtlas( tex, { 2, 1, 1, 1 }, 3, pos + CVector( size.x, 0.5 ), CVector( 0.5, size.y - 1 ), CVector( 0, 0 ),
				color );

	// Middle
	ImageAtlas( tex, { 1, 1, 1, 1 }, 3, pos + CVector( 0.5, 0.5 ), CVector( size.x - 0.5, size.y - 1 ), CVector( 0, 0 ),
				color );
}

void CGui::Crosshair() { Image( m_pCrosshairTex, m_vScreenCentre, CVector( 3, 3 ), CVector( 0.5, 0.5 ) ); }

// Returns the string in the event that 'RETURN' is pressed.
// Outputs nullptr if nothing.
// id can be shared between multiple text inputs if they're for the same data.
const char *CGui::TextInput( int id, CVector pos )
{
	std::string text = m_textBuffers[id];

	Label( ( text + ( ( ( m_iTick / 8 ) % 2 == 0 ) ? "_" : "" ) ).c_str(), pos );

	if ( m_pInputMan->m_cTypeKey != nullptr )
	{
		text += m_pInputMan->m_cTypeKey;
	}

	if ( m_pInputMan->m_clipboard.size() )
		text += m_pInputMan->m_clipboard;

	if ( m_pInputMan->m_bKeyboardState[KBD_BACKSPACE] && !m_pInputMan->m_bOldKeyboardState[KBD_BACKSPACE] )
	{
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

	if ( m_pInputMan->m_bKeyboardState[KBD_RETURN] && !m_pInputMan->m_bOldKeyboardState[KBD_RETURN] )
		return m_textBuffers[id].c_str();
	else
		return nullptr;
}

const char *CGui::SelectableTextInput( int id, CVector pos, CVector size, CTexture *pTex )
{
	pos	 = GetInScreen( pos );
	size = size * GUIUNIT;

	if ( pTex == nullptr )
		pTex = m_pTextInpTex;

	Colour color( 1, 1, 1 );
	Colour textColor( 1, 1, 1 );

	if ( RegionHit( pos, size ) )
	{
		m_iHotItem = id;
		color	   = Colour( 0.75, 0.75, 1 );
		textColor  = Colour( 1, 1, 0.75 );

		if ( m_iActiveItem == 0 && ( m_iMouseState == IN_LEFT_MOUSE ) )
		{
			m_iActiveItem	= id;
			m_iKeyboardItem = id;
		}
	}

	if ( m_iKeyboardItem != id )
		Label( m_textBuffers[id].c_str(), ( pos / GUIUNIT ) + CVector( 0.5, 0.5 ), textColor );
	else
	{
		TextInput( id, ( pos / GUIUNIT ) + CVector( 0.5, 0.5 ) );
	}

	Image9Rect( pTex, pos / GUIUNIT, size / GUIUNIT, Colour( 1, 1, 1 ) );

	return nullptr;
}

bool CGui::Slider( int id, CVector pos, CVector size, int max, int &value )
{
	pos	 = GetInScreen( pos );
	size = size * GUIUNIT;

	Image9Rect( m_pSliderTex, pos / GUIUNIT, size / GUIUNIT, Color( 1, 1, 1 ) );

	float ypos = ( ( size.y - 2 * GUIUNIT ) * value ) / max;

	Image( m_pThumbTex, ( pos + CVector( 0, ypos ) ) / GUIUNIT, CVector( 2, 2 ), CVector( 0, 0 ), Color( 1, 1, 1 ) );

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
		float p = ( m_vMousePos.y * -1 ) + pos.y;
		if ( p < 0 )
			p = 0;
		if ( p > size.y )
			p = size.y;
		int val = ( p * max ) / size.y;

		if ( val != value )
		{
			value = val;
			return true;
		}
	}

	return false;
}
bool CGui::HorzSlider( int id, CVector pos, CVector size, int max, int &value )
{
	pos	 = GetInScreen( pos );
	size = size * GUIUNIT;

	Image9Rect( m_pSliderTex, pos / GUIUNIT, size / GUIUNIT, Color( 1, 1, 1 ) );

	float xpos = ( ( size.x - 2 * GUIUNIT ) * value ) / max;

	Image( m_pThumbTex, ( pos + CVector( xpos, 0 ) ) / GUIUNIT, CVector( 2, 2 ), CVector( 0, 0 ), Color( 1, 1, 1 ) );

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
		float p = m_vMousePos.x - pos.x;
		if ( p < 0 )
			p = 0;
		if ( p > size.x )
			p = size.x;
		int val = ( p * max ) / size.x;

		if ( val != value )
		{
			value = val;
			return true;
		}
	}

	return false;
}

bool CGui::CheckBox( int id, CVector pos, CVector size, bool &value )
{
	pos	 = GetInScreen( pos );
	size = size * GUIUNIT;

	CTexture *tex = m_pUncheckedTex;
	if (value)
		tex = m_pCheckedTex;

	Image(tex, pos / GUIUNIT, size / GUIUNIT);

	if ( RegionHit( pos, size ) )
	{
		m_iHotItem = id;

		if ( m_iActiveItem == 0 && ( m_iMouseState == IN_LEFT_MOUSE ) )
		{
			m_iActiveItem = id;

			value = !value;
			return true;
		}
	}

	return false;
}