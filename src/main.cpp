#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_keyboard.h>

#include "sdlstuff/sdlwindow.h"

#include "vector.h"
#include "player.h"
// #include "chunk.h"
#include "texturemanager.h"
// #include "chunkmanager.h"

#include <fstream>
#include <iostream>

#include <glad/glad.h>

int main (int argc, char* args[]) {
	// Initialize SDL systems
	if(SDL_Init( SDL_INIT_VIDEO )) {
		printf("SDL could not initialize! SDL_Error: %s\n",
					SDL_GetError());
		return -1;
	}
	SDL_GL_LoadLibrary(NULL);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	GameWindow window("VoxelThingYeah", Vector(800,600));
	const int WIDTH = window.GetSize().x;
	const int HEIGHT = window.GetSize().y;

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		// TODO: Use SDL rendering and display an error on-screen
		printf("Cannot load Glad (SOMEHOW!?)\n");
		return -1;
	}

	printf("---- OpenGL Info ----\nVendor: %s\nRenderer: %s\nVersion: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

	// Create input manager and give the window a pointer to it
	InputManager input;
	window.inputMan = &input;

	// Chunk testChunk;
	// ChunkManager chunkMan;

	Player plyr = Player();
	plyr.inputMan = &input;

	TextureManager texman;
	Texture terrainpng = texman.LoadTexture("terrain.png");

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