#ifndef SDLWINDOWK_H
#define SDLWINDOWK_H

#include "window.hpp"

#pragma once

#include <memory>

#include <SDL2/SDL.h>

#include "inputmanager.hpp"
#include "utility/vector.hpp"

class CGameWindow
{
  private:
	SDL_Cursor **m_cursors;

  public:
	CGameWindow( const char *title = "GameWindow", Vector3i size = Vector3i( 800, 600 ), bool resizeable = false,
				 Vector3i minSize = Vector3i( 0, 0 ) ); // SDL_CreateWindow
	~CGameWindow();										// SDL_DestroyWindow

	// Gets the OpenGL Context
	void GatherCTX();

	void SetIcon( const char *texName );

	// SDL_HideWindow / SDL_ShowWindow
	bool IsVisible();		   // getter(?)
	void SetVisible( bool v ); // setter(?)

	bool IsFocused();

	// SDL_SetWindowTitle / SDL_GetWindowTitle
	void SetTitle( const char *title );
	const char *GetTitle();

	// Z Ignored
	// SDL_SetWindowSize / SDL_GetWindowSize
	Vector3i GetSize();
	void SetSize( Vector3i s );
	// Z Ignored
	// SDL_SetWindowPos / SDL_GetWindowPos
	Vector3i GetPos();
	void SetPos( Vector3i p );

	void SetMouseVisibility( bool bVisible );
	bool GetMouseVisibility();

	// Displays an error message somehow, Use for when shit hits the fans and we need to exit immediately
	void Panic( const char *err );

	float GetSPF();
	double GetTime();

	unsigned int m_iTick;
	unsigned int m_iLastTick;

	unsigned int m_iFrameTicks;
	unsigned int m_iFramesInTheLastSecond;
	float m_fSecondsPerFrame;

	double m_dDelta;

	unsigned int GetMS();

	void PollEvents();

	void CaptureMouse();

	void SwapBuffers();

	void UpdateSize();

	std::unique_ptr<SDL_Window, decltype( &SDL_DestroyWindow )> m_pInternalWindow;
	SDL_GLContext m_glctx;

	bool m_bShouldClose;
	bool m_bSizeChanged;

	// Pointer to Input manager
	CInputManager *m_pInputMan = nullptr;
};

#endif