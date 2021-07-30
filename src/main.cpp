#include <stdio.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

SDL_Window* window = NULL;
const int WIDTH = 640;
const int HEIGHT = 480;
int main (int argc, char* args[]) {
	// Initialize SDL systems
	if(SDL_Init( SDL_INIT_VIDEO )) {
		printf("SDL could not initialize! SDL_Error: %s\n",
					SDL_GetError());
		return -1;
	}
	//Create a window
	window = SDL_CreateWindow("VoxelThingYeah",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH, HEIGHT,
		SDL_WINDOW_SHOWN);
	if(window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n",
					SDL_GetError());
		return -1;
	}

	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(window, &wmi)) {
		return 1;
	}

	bgfx::PlatformData pd;
	pd.ndt = wmi.info.x11.display;
	pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;

	bgfx::setPlatformData(pd);

	bgfx::init();
	bgfx::reset(WIDTH, HEIGHT, BGFX_RESET_VSYNC);
	bgfx::setViewRect(0, 0, 0, uint16_t(WIDTH), uint16_t(HEIGHT));
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x008080ff, 1.0f, 0);
	bgfx::touch(0);


	bool run = true;
	SDL_Event currentEvent;
	while(run) {
		while(SDL_PollEvent(&currentEvent) != 0) {
			if(currentEvent.type == SDL_QUIT) {
				run = false;
			}
		}

		bgfx::frame();
	}

	// Free up window
	SDL_DestroyWindow(window);

	bgfx::shutdown();
	// Shutdown SDL
	SDL_Quit();

	return 0;
}