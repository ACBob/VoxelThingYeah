// Implements window with SDL

#include "../window.h"
#include <cstdio>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <memory>


// TODO: OO :trollface:
SDL_Window *_WINDOW;
SDL_SysWMinfo wmi;

BobWindow::BobWindow(const char *title, Vector size)
{
	_WINDOW = SDL_CreateWindow("VoxelThingYeah",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		size.x, size.y,
		SDL_WINDOW_SHOWN);

	if (_WINDOW == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n",
					SDL_GetError());
	}

	SDL_VERSION(&wmi.version);

	nativeWindow = &wmi.info.x11.window;
	#ifdef __linux__
	nativeDisplayType = &wmi.info.x11.display;
	#endif
}
BobWindow::~BobWindow()
{
	SDL_DestroyWindow(_WINDOW);
}