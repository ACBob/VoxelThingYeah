// Implements window with SDL

#include "sdlwindow.hpp"
#include <cstdio>
#include <iostream>
#include <memory>

CGameWindow::CGameWindow( const char *title, CVector size, bool resizeable )
	: m_pInternalWindow( nullptr, &SDL_DestroyWindow ), m_iTick( 0 ), m_iFrameTicks( 0 ), m_dDelta( 0 ),
	  m_iFramesInTheLastSecond( 0 ), m_fSecondsPerFrame( 0.0f ), m_pInputMan( nullptr ), m_bShouldClose( false )
{
	m_pInternalWindow.reset(
		SDL_CreateWindow( "VoxelThingYeah", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y,
						  SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | ( resizeable ? SDL_WINDOW_RESIZABLE : 0 ) ) );

	// if (resizeable);
	// 	flags |= SDL_WINDOW_RESIZABLE;

	if ( m_pInternalWindow == NULL )
	{
		printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
	}

	m_glctx = SDL_GL_CreateContext( m_pInternalWindow.get() );

	SDL_SetRelativeMouseMode( SDL_TRUE );
}
CGameWindow::~CGameWindow() {}

void CGameWindow::SwapBuffers() { SDL_GL_SwapWindow( m_pInternalWindow.get() ); }

bool CGameWindow::IsVisible()
{
	return !( ( SDL_GetWindowFlags( m_pInternalWindow.get() ) & SDL_WINDOW_HIDDEN ) == SDL_WINDOW_HIDDEN );
}
void CGameWindow::SetVisible( bool visible )
{
	if ( !visible )
		SDL_HideWindow( m_pInternalWindow.get() );
	else
		SDL_ShowWindow( m_pInternalWindow.get() );
}

bool CGameWindow::IsFocused()
{
	return IsVisible() &&
		   ( SDL_GetWindowFlags( m_pInternalWindow.get() ) & SDL_WINDOW_INPUT_FOCUS ) == SDL_WINDOW_INPUT_FOCUS;
}

const char *CGameWindow::GetTitle() { return SDL_GetWindowTitle( m_pInternalWindow.get() ); }
void CGameWindow::SetTitle( const char *title ) { SDL_SetWindowTitle( m_pInternalWindow.get(), title ); }

CVector CGameWindow::GetSize()
{
	int x, y;
	SDL_GetWindowSize( m_pInternalWindow.get(), &x, &y );
	return CVector( x, y );
}
void CGameWindow::SetSize( CVector size ) { SDL_SetWindowSize( m_pInternalWindow.get(), size.x, size.y ); }

CVector CGameWindow::GetPos()
{
	int x, y;
	SDL_GetWindowPosition( m_pInternalWindow.get(), &x, &y );
	return CVector( x, y );
}
void CGameWindow::SetPos( CVector pos ) { SDL_SetWindowPosition( m_pInternalWindow.get(), pos.x, pos.y ); }

float CGameWindow::GetSPF()
{
	m_iFramesInTheLastSecond++;
	if ( m_iFrameTicks < SDL_GetTicks() - 1000 ) // Has it been a second
	{
		m_iFrameTicks			 = m_iTick;
		m_fSecondsPerFrame		 = 1000.0f / float( m_iFramesInTheLastSecond );
		m_iFramesInTheLastSecond = 0;
	}
	return m_fSecondsPerFrame;
}

unsigned int CGameWindow::GetMS()
{
	m_iTick = SDL_GetTicks();
	return m_iTick;
}
double CGameWindow::GetTime()
{
	m_iTick = SDL_GetTicks();
	return m_iTick / 1000.0f;
}

const int scancodeToStateIndex[] = {
	SDL_SCANCODE_SPACE,		' ',

	SDL_SCANCODE_0,			'0',		SDL_SCANCODE_1,		 '1',		SDL_SCANCODE_2,			'2',
	SDL_SCANCODE_3,			'3',		SDL_SCANCODE_4,		 '4',		SDL_SCANCODE_5,			'5',
	SDL_SCANCODE_6,			'6',		SDL_SCANCODE_7,		 '7',		SDL_SCANCODE_8,			'8',
	SDL_SCANCODE_9,			'9',

	SDL_SCANCODE_SEMICOLON, ';',

	SDL_SCANCODE_A,			'A',		SDL_SCANCODE_B,		 'B',		SDL_SCANCODE_C,			'C',
	SDL_SCANCODE_D,			'D',		SDL_SCANCODE_E,		 'E',		SDL_SCANCODE_F,			'F',
	SDL_SCANCODE_G,			'G',		SDL_SCANCODE_H,		 'H',		SDL_SCANCODE_I,			'I',
	SDL_SCANCODE_J,			'J',		SDL_SCANCODE_K,		 'K',		SDL_SCANCODE_L,			'L',
	SDL_SCANCODE_M,			'M',		SDL_SCANCODE_N,		 'N',		SDL_SCANCODE_O,			'O',
	SDL_SCANCODE_P,			'P',		SDL_SCANCODE_Q,		 'Q',		SDL_SCANCODE_R,			'R',
	SDL_SCANCODE_S,			'S',		SDL_SCANCODE_T,		 'T',		SDL_SCANCODE_U,			'U',
	SDL_SCANCODE_V,			'V',		SDL_SCANCODE_W,		 'W',		SDL_SCANCODE_X,			'X',
	SDL_SCANCODE_Y,			'Y',		SDL_SCANCODE_Z,		 'Z',

	SDL_SCANCODE_ESCAPE,	KBD_ESCAPE, SDL_SCANCODE_LSHIFT, KBD_SHIFT, SDL_SCANCODE_BACKSPACE, KBD_BACKSPACE,
	SDL_SCANCODE_RETURN,	KBD_RETURN,
};

void CGameWindow::PollEvents()
{
	m_pInputMan->m_vMouseMovement = CVector( 0, 0 );

	m_pInputMan->m_iOldMouseState = m_pInputMan->m_iMouseState;
	m_pInputMan->m_iMouseState	  = 0;

	SDL_Event currentEvent;
	while ( SDL_PollEvent( &currentEvent ) != 0 )
	{
		switch ( currentEvent.type )
		{
			case SDL_QUIT: // Handle quitting directly
				m_bShouldClose = true;
				break;

			case SDL_MOUSEMOTION:
				// HACK HACK HACK HACK: ignore mouse events while invisible
				if ( !IsFocused() )
					continue;
				m_pInputMan->m_vMouseMovement =
					m_pInputMan->m_vMouseMovement + CVector( currentEvent.motion.xrel, currentEvent.motion.yrel );
				m_pInputMan->m_vMousePos = CVector( currentEvent.motion.x, currentEvent.motion.y );
				break;

			case SDL_WINDOWEVENT:
				m_bSizeChanged = true;
				break;

			case SDL_MOUSEWHEEL:
				m_pInputMan->m_iMouseState |= ( currentEvent.wheel.y > 0 ) ? IN_WHEEL_UP : IN_WHEEL_DOWN;
				break;
		}
	}

	// Fill the input manager
	const uint8_t *state = SDL_GetKeyboardState( NULL );
	for ( int i = 0; i < sizeof( scancodeToStateIndex ) / sizeof( int ); i += 2 )
	{
		int scanCode							   = scancodeToStateIndex[i];
		int convCode							   = scancodeToStateIndex[i + 1];
		m_pInputMan->m_bOldKeyboardState[convCode] = m_pInputMan->m_bKeyboardState[convCode];
		m_pInputMan->m_bKeyboardState[convCode]	   = ( state[scanCode] == 1 );
	}

	// Set the mouseState for buttons
	unsigned int buttons = SDL_GetMouseState( NULL, NULL );
	if ( ( buttons & SDL_BUTTON_LMASK ) != 0 )
	{
		m_pInputMan->m_iMouseState = m_pInputMan->m_iMouseState | IN_LEFT_MOUSE;
	}
	if ( ( buttons & SDL_BUTTON_RMASK ) != 0 )
	{
		m_pInputMan->m_iMouseState = m_pInputMan->m_iMouseState | IN_RIGHT_MOUSE;
	}
}

void CGameWindow::CaptureMouse()
{
	CVector size = GetSize();
	SDL_WarpMouseInWindow( m_pInternalWindow.get(), size.x / 2, size.y / 2 );
}