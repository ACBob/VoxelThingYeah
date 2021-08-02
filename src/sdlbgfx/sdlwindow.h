#ifndef SDLWINDOWK_H
#define SDLWINDOWK_H

#include "../window.h"

#pragma once

#include <memory>

#include <SDL.h>

class GameWindow : public BobWindow
{
	public:
		GameWindow(const char *title = "GameWindow", Vector size = Vector(800,600)); // SDL_CreateWindow
		~GameWindow(); // SDL_DestroyWindow

		// SDL_HideWindow / SDL_ShowWindow
		bool IsVisible(); // getter(?)
		void SetVisible(bool v); // setter(?)

		// SDL_SetWindowTitle / SDL_GetWindowTitle
		void SetTitle(const char *title);
		const char *GetTitle();

		// Z Ignored
		// SDL_SetWindowSize / SDL_GetWindowSize
		Vector GetSize();
		void SetSize(Vector s);
		// Z Ignored
		// SDL_SetWindowPos / SDL_GetWindowPos
		Vector GetPos();
		void SetPos(Vector p);

		std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> internalWindow;
};

#endif