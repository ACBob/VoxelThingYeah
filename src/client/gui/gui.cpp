// An ImGui
// TODO: Abstract OpenGL away from this, making it renderer agnostic
//       (Store like a mesh, have some kind of renderer)

#include "gui.hpp"

#include <glad/glad.h>

// The whole coordinate system is based on these GUIUUNITs.
// When we start to dynamically rescale the GUI based on resolution this will come in handy. for now, 16px.
#define GUIUNIT 16

#define TEXTINTEXWIDTH 8
#define TEXTINTEXHEIGHT 9
#define TEXTRATIO 1.125

#define TEXTWIDTH GUIUNIT *TEXTRATIO
#define TEXTHEIGHT GUIUNIT

// I am not supporting weird configurations
#define TEXTTILES 16

CGui::CGui( int screenW, int screenH ) : mouseState( IN_NO_MOUSE ), activeItem( 0 ), hotItem( 0 )
{
	// OpenGl
	{
		glGenVertexArrays( 1, &vao );
		glGenBuffers( 1, &vbo );

		glBindVertexArray( vao );

		glBindBuffer( GL_ARRAY_BUFFER, vbo );
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

	textTex	   = materialSystem::LoadTexture( "font.png" );
	textShader = shaderSystem::LoadShader( "shaders/text.vert", "shaders/text.frag" );

	textBuffers = {};

	Resize( screenW, screenH );
}

void CGui::Resize( int x, int y )
{
	screenCentre	 = CVector( ( x * 0.5 ) / GUIUNIT, ( y * 0.5 ) / GUIUNIT );
	screenDimensions = CVector( x, y );
}

CGui::~CGui()
{
	// OpenGl
	{
		glDeleteBuffers( 1, &vao );
		glDeleteVertexArrays( 1, &vbo );
	}
}

void CGui::Update()
{
	if ( mouseState == IN_NO_MOUSE )
		activeItem = 0;

	mousePos   = inputMan->mousePos;
	mouseState = inputMan->mouseState;
	// else if (activeItem == 0)
	// 	activeItem = -1;

	hotItem = 0;

	// Render
	{
		int depthFunc;
		glGetIntegerv( GL_DEPTH_FUNC, &depthFunc );
		glDepthFunc( GL_ALWAYS );

		textShader->Use();

		glBindVertexArray( vao );
		// Text
		{
			glBindTexture( GL_TEXTURE_2D, textTex->id );

			glBindBuffer( GL_ARRAY_BUFFER, vbo );
			glBufferData( GL_ARRAY_BUFFER, textVertiecs.size() * sizeof( CGui::Vertex ), textVertiecs.data(),
						  GL_DYNAMIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glDrawArrays( GL_TRIANGLES, 0, textVertiecs.size() );

			glBindTexture( GL_TEXTURE_2D, 0 );
		}
		glBindVertexArray( 0 );
		// Images
		glBindVertexArray( vao );
		{
			for ( CGui::_Image img : images )
			{
				glBindTexture( GL_TEXTURE_2D, img._tex->id );
				glBindBuffer( GL_ARRAY_BUFFER, vbo );
				glBufferData( GL_ARRAY_BUFFER, img.vertices.size() * sizeof( CGui::Vertex ), img.vertices.data(),
							  GL_DYNAMIC_DRAW );
				glBindBuffer( GL_ARRAY_BUFFER, 0 );
				glDrawArrays( GL_TRIANGLES, 0, img.vertices.size() );
				glBindTexture( GL_TEXTURE_2D, 0 );
			}
		}
		glBindVertexArray( 0 );

		glDepthFunc( depthFunc );
	}

	// Clear our vertices
	textVertiecs.clear();
	// Clear our images
	images.clear();
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
std::vector<CGui::Vertex> CGui::GetCharQuad( const char *c, CVector pos, CVector size, Colour color )
{
	float x, y;
	x = ( int( *c - ' ' ) % TEXTTILES ) * TEXTINTEXWIDTH;
	y = ( int( *c - ' ' ) / TEXTTILES ) * TEXTINTEXHEIGHT;

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

bool CGui::RegionHit( CVector pos, CVector size )
{
	if ( mousePos.x < pos.x || mousePos.y < pos.y ||

		 mousePos.x > pos.x + size.x || mousePos.y > pos.y + size.y )
		return false;
	return true;
}

CVector CGui::GetInScreen( CVector pos )
{
	pos = pos * GUIUNIT;

	if ( pos.x < 0 )
		pos.x += screenDimensions.x;
	if ( pos.y < 0 )
		pos.y += screenDimensions.y;

	return pos;
}

int CGui::Button( int id, CVector pos, CVector size )
{
	pos	 = GetInScreen( pos );
	size = size * GUIUNIT;

	int returnCode = 0;

	Colour color = Colour( 0.5, 0.5, 0.5 );

	// Check & set State
	if ( RegionHit( pos, size ) )
	{
		hotItem = id;
		color	= Colour( 0.75, 0.75, 0.75 );
		if ( mouseState != 0 )
			NULL; // Breakpoint
		if ( activeItem == 0 && ( mouseState == IN_LEFT_MOUSE ) )
		{
			activeItem = id;
		}
	}

	if ( mouseState == IN_NO_MOUSE && hotItem == id && activeItem == id )
	{
		returnCode = 1;
		color	   = Colour( 0.75, 0.75, 1 );
	}

	// Render
	// OpenGl
	{
		// Get vertices
		std::vector<CGui::Vertex> g = GetQuad( pos, size, color );
		std::copy( g.begin(), g.end(), std::back_inserter( textVertiecs ) );
	}

	return returnCode;
}

void CGui::Label( const char *text, CVector pos, Colour color )
{
	pos = GetInScreen( pos );

	// Render
	// OpenGl
	{
		int i = 0;

		while ( text[i] != '\0' )
		{
			// Get vertices
			std::vector<CGui::Vertex> g = GetCharQuad( &text[i], pos, CVector( TEXTWIDTH, TEXTHEIGHT ), color );
			std::copy( g.begin(), g.end(), std::back_inserter( textVertiecs ) );

			pos = pos + CVector( TEXTWIDTH, 0 );

			i++;
		}
	}
}

void CGui::Image( CTexture *tex, CVector pos, CVector size, CVector origin )
{
	pos	 = pos * GUIUNIT;
	size = size * GUIUNIT;

	{
		CGui::_Image img;
		img.vertices = GetQuad( pos - ( size * origin ), size, Colour( 1, 1, 1 ) );
		img._tex	 = tex;
		images.push_back( img );
	}
}

void CGui::ImageAtlas( CTexture *tex, Atlas atlas, float atlasDivisions, CVector pos, CVector size, CVector origin )
{
	pos	 = GetInScreen( pos );
	size = size * GUIUNIT;

	{
		CGui::_Image img;
		img.vertices = GetQuad(
			pos - ( size * origin ), size, Colour( 1, 1, 1 ), { atlas.x / atlasDivisions, atlas.y / atlasDivisions },
			{ ( atlas.x + atlas.sizex ) / atlasDivisions, ( atlas.y + atlas.sizey ) / atlasDivisions } );
		img._tex = tex;
		images.push_back( img );
	}
}

// Returns the string in the event that 'RETURN' is pressed.
// Outputs nullptr if nothing.
// id can be shared between multiple text inputs if they're for the same data.
const char *CGui::TextInput( int id, CVector pos )
{
	std::string text = textBuffers[id];

	Label( text.c_str(), pos );

	// TODO: I hate this
	// FIXME: Doesn't account for keyboard, get input manager to have some kind of built-in thing that talks to the
	// window and does it for us! AHJSDHJASHJKDASJKDJKASDHKAHKJDSJk
	for ( int i = ' '; i <= 'Z'; i++ )
	{
		if ( inputMan->keyboardState[i] && !inputMan->oldKeyboardState[i] )
		{
			if ( inputMan->keyboardState[KBD_SHIFT] )
			{
				switch ( i )
				{
					case ';':
						text += ':';
						continue;
						break;
					case '1':
						text += '!';
						continue;
						break;
					case '2':
						text += '"';
						continue;
						break;
					case '9':
						text += '(';
						continue;
						break;
					case '0':
						text += ')';
						continue;
						break;
				}
				text += i;
			}
			else if ( i >= 'A' && i <= 'Z' )
				text += i + 32;
			else
				text += i;
		}
	}

	if ( inputMan->keyboardState[KBD_BACKSPACE] && !inputMan->oldKeyboardState[KBD_BACKSPACE] )
	{
		if ( text.length() )
			text.pop_back();
	}

	textBuffers[id] = text;

	if ( inputMan->keyboardState[KBD_RETURN] && !inputMan->oldKeyboardState[KBD_RETURN] )
		return textBuffers[id].c_str();
	else
		return nullptr;
}