#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_keyboard.h>

#include "sdlstuff/sdlwindow.h"

#include "vector.h"
#include "player.h"
// #include "chunk.h"
// #include "texturemanager.h"
// #include "chunkmanager.h"

#include <fstream>
#include <iostream>

#include <GL/gl.h>

int main (int argc, char* args[]) {
	// Initialize SDL systems
	if(SDL_Init( SDL_INIT_VIDEO )) {
		printf("SDL could not initialize! SDL_Error: %s\n",
					SDL_GetError());
		return -1;
	}

	GameWindow window("VoxelThingYeah", Vector(800,600));
	const int WIDTH = window.GetSize().x;
	const int HEIGHT = window.GetSize().y;

	// Create input manager and give the window a pointer to it
	InputManager input;
	window.inputMan = &input;

	// Chunk testChunk;
	// ChunkManager chunkMan;

	Player plyr = Player();
	plyr.inputMan = &input;

	// TextureManager texman;
	// Texture terrainpng = texman.LoadTexture("terrain.png");

	glClearColor(0.5f, 0.0f, 0.5f, 1.0f);
	glViewport(0, 0, WIDTH, HEIGHT);

	// Get relative mouse change from MouseMove

	while(!window.shouldClose) {
		window.PollEvents();

		if (window.IsVisible())
			window.CaptureMouse();

		// Entity handling go here

		plyr.Update();

		// Rendering right at the end
		glClear(GL_COLOR_BUFFER_BIT);

		// testChunk.Render(prg.idx);

		window.SwapBuffers();
	}
	// Shutdown SDL
	SDL_Quit();

	return 0;
}