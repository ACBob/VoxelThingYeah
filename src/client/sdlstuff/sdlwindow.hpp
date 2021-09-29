#ifndef SDLWINDOWK_H
#define SDLWINDOWK_H

#include "window.hpp"

#pragma once

#include <memory>

#include <SDL.h>

#include "rendering/texturemanager.hpp"

class CGameWindow : public CBobWindow
{
	private:
		CTexture *m_pIcon;

  public:
	CGameWindow( const char *title = "GameWindow", CVector size = CVector( 800, 600 ),
				 bool resizeable = false ); // SDL_CreateWindow
	~CGameWindow();							// SDL_DestroyWindow

	// Gets the OpenGL Context
	void GatherCTX();

	void SetIcon(const char *texName);

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

	// Displays an error message somehow, Use for when shit hits the fans and we need to exit immediately
	void Panic(const char *err);

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