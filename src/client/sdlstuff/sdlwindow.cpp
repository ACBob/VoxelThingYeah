// Implements window with SDL

#include "sdlwindow.hpp"
#include <cstdio>
#include <iostream>
#include <memory>

CGameWindow::CGameWindow( const char *title, CVector size, bool resizeable )
	: internalWindow( nullptr, &SDL_DestroyWindow ), tick( 0 ), frameTicks( 0 ), delta( 0 ), framesInTheLastSecond( 0 ),
	  secondsPerFrame( 0.0f ), inputMan( nullptr ), shouldClose( false )
{
	internalWindow.reset(
		SDL_CreateWindow( "VoxelThingYeah", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y,
						  SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | ( resizeable ? SDL_WINDOW_RESIZABLE : 0 ) ) );

	// if (resizeable);
	// 	flags |= SDL_WINDOW_RESIZABLE;

	if ( internalWindow == NULL )
	{
		printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
	}

	glctx = SDL_GL_CreateContext( internalWindow.get() );

	SDL_SetRelativeMouseMode( SDL_TRUE );
}
CGameWindow::~CGameWindow() {}

void CGameWindow::SwapBuffers() { SDL_GL_SwapWindow( internalWindow.get() ); }

bool CGameWindow::IsVisible()
{
	return !( ( SDL_GetWindowFlags( internalWindow.get() ) & SDL_WINDOW_HIDDEN ) == SDL_WINDOW_HIDDEN );
}
void CGameWindow::SetVisible( bool visible )
{
	if ( !visible )
		SDL_HideWindow( internalWindow.get() );
	else
		SDL_ShowWindow( internalWindow.get() );
}

bool CGameWindow::IsFocused()
{
	return IsVisible() &&
		   ( SDL_GetWindowFlags( internalWindow.get() ) & SDL_WINDOW_INPUT_FOCUS ) == SDL_WINDOW_INPUT_FOCUS;
}

const char *CGameWindow::GetTitle() { return SDL_GetWindowTitle( internalWindow.get() ); }
void CGameWindow::SetTitle( const char *title ) { SDL_SetWindowTitle( internalWindow.get(), title ); }

CVector CGameWindow::GetSize()
{
	int x, y;
	SDL_GetWindowSize( internalWindow.get(), &x, &y );
	return CVector( x, y );
}
void CGameWindow::SetSize( CVector size ) { SDL_SetWindowSize( internalWindow.get(), size.x, size.y ); }

CVector CGameWindow::GetPos()
{
	int x, y;
	SDL_GetWindowPosition( internalWindow.get(), &x, &y );
	return CVector( x, y );
}
void CGameWindow::SetPos( CVector pos ) { SDL_SetWindowPosition( internalWindow.get(), pos.x, pos.y ); }

float CGameWindow::GetSPF()
{
	framesInTheLastSecond++;
	if ( frameTicks < SDL_GetTicks() - 1000 ) // Has it been a second
	{
		frameTicks			  = tick;
		secondsPerFrame		  = 1000.0f / float( framesInTheLastSecond );
		framesInTheLastSecond = 0;
	}
	return secondsPerFrame;
}

unsigned int CGameWindow::GetMS()
{
	tick = SDL_GetTicks();
	return tick;
}
double CGameWindow::GetTime()
{
	tick = SDL_GetTicks();
	return tick / 1000.0f;
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
	inputMan->mouseMovement = CVector( 0, 0 );

	inputMan->oldMouseState = inputMan->mouseState;
	inputMan->mouseState	= 0;

	SDL_Event currentEvent;
	while ( SDL_PollEvent( &currentEvent ) != 0 )
	{
		switch ( currentEvent.type )
		{
			case SDL_QUIT: // Handle quitting directly
				shouldClose = true;
				break;

			case SDL_MOUSEMOTION:
				// HACK HACK HACK HACK: ignore mouse events while invisible
				if ( !IsFocused() )
					continue;
				inputMan->mouseMovement =
					inputMan->mouseMovement + CVector( currentEvent.motion.xrel, currentEvent.motion.yrel );
				inputMan->mousePos = CVector( currentEvent.motion.x, currentEvent.motion.y );
				break;

			case SDL_WINDOWEVENT:
				sizeChanged = true;
				break;

			case SDL_MOUSEWHEEL:
				inputMan->mouseState |= ( currentEvent.wheel.y > 0 ) ? IN_WHEEL_UP : IN_WHEEL_DOWN;
				break;
		}
	}

	// Fill the input manager
	const uint8_t *state = SDL_GetKeyboardState( NULL );
	for ( int i = 0; i < sizeof( scancodeToStateIndex ) / sizeof( int ); i += 2 )
	{
		int scanCode						 = scancodeToStateIndex[i];
		int convCode						 = scancodeToStateIndex[i + 1];
		inputMan->oldKeyboardState[convCode] = inputMan->keyboardState[convCode];
		inputMan->keyboardState[convCode]	 = ( state[scanCode] == 1 );
	}

	// Set the mouseState for buttons
	unsigned int buttons = SDL_GetMouseState( NULL, NULL );
	if ( ( buttons & SDL_BUTTON_LMASK ) != 0 )
	{
		inputMan->mouseState = inputMan->mouseState | IN_LEFT_MOUSE;
	}
	if ( ( buttons & SDL_BUTTON_RMASK ) != 0 )
	{
		inputMan->mouseState = inputMan->mouseState | IN_RIGHT_MOUSE;
	}
}

void CGameWindow::CaptureMouse()
{
	CVector size = GetSize();
	SDL_WarpMouseInWindow( internalWindow.get(), size.x / 2, size.y / 2 );
}