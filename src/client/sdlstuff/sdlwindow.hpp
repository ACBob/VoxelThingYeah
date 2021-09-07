#ifndef SDLWINDOWK_H
#define SDLWINDOWK_H

#include "window.hpp"

#pragma once

#include <memory>

#include <SDL.h>

class CGameWindow : public CBobWindow
{
  public:
	CGameWindow( const char *title = "GameWindow", CVector size = CVector( 800, 600 ),
				bool resizeable = false ); // SDL_CreateWindow
	~CGameWindow();						   // SDL_DestroyWindow

	// SDL_HideWindow / SDL_ShowWindow
	bool IsVisible();		   // getter(?)
	void SetVisible( bool v ); // setter(?)

	bool IsFocused();

	// SDL_SetWindowTitle / SDL_GetWindowTitle
	void SetTitle( const char *title );
	const char *GetTitle();

	// Z Ignored
	// SDL_SetWindowSize / SDL_GetWindowSize
	CVector GetSize();
	void SetSize( CVector s );
	// Z Ignored
	// SDL_SetWindowPos / SDL_GetWindowPos
	CVector GetPos();
	void SetPos( CVector p );

	float GetSPF();
	double GetTime();

	unsigned int tick;
	unsigned int lastTick;

	unsigned int frameTicks;
	unsigned int framesInTheLastSecond;
	float secondsPerFrame;

	double delta;

	unsigned int GetMS();

	void PollEvents();

	void CaptureMouse();

	void SwapBuffers();

	void UpdateSize();

	std::unique_ptr<SDL_Window, decltype( &SDL_DestroyWindow )> internalWindow;
	SDL_GLContext glctx;

	bool shouldClose;
	bool sizeChanged;

	// Pointer to Input manager
	CInputManager *inputMan = nullptr;
};

#endif