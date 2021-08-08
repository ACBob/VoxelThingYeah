#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_keyboard.h>

#include "sdlstuff/sdlwindow.h"

#include "utility/vector.h"
#include "player.h"
#include "world/chunk.h"
#include "rendering/texturemanager.h"
#include "rendering/shadermanager.h"
#include "world/chunkmanager.h"
#include "gui/gui.h"

#include <fstream>
#include <iostream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void GLAPIENTRY GlMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
				type, severity, message );
}

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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	GameWindow window("VoxelThingYeah", Vector(1280,720), true);
	const int WIDTH = window.GetSize().x;
	const int HEIGHT = window.GetSize().y;

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		// TODO: Use SDL rendering and display an error on-screen
		printf("Cannot load Glad (SOMEHOW!?)\n");
		return -1;
	}

	printf("---- OpenGL Info ----\nVendor: %s\nRenderer: %s\nVersion: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

	glEnable              ( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( GlMessageCallback, 0 );

	// Create input manager and give the window a pointer to it
	InputManager input;
	window.inputMan = &input;

	ChunkManager chunkMan;

	Player plyr = Player();
	plyr.inputMan = &input;

	TextureManager texman;
	Texture* terrainpng = texman.LoadTexture("terrain.png");

	Shader genericShader = Shader("shaders/generic.vert", "shaders/generic.frag");
	Shader textShader = Shader("shaders/text.vert", "shaders/text.frag");

	GUI gui(&texman);
	gui.inputMan = &input;

	glClearColor(0.5f, 0.0f, 0.5f, 1.0f);
	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE); 
	glCullFace(GL_FRONT);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Get relative mouse change from MouseMove
	
	while(!window.shouldClose) {
		window.PollEvents();

		if (window.IsVisible())
			window.CaptureMouse();

		// Entity handling go here

		plyr.Update(&chunkMan);

		// Rendering right at the end
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		genericShader.Use();

		glBindTexture(GL_TEXTURE_2D, terrainpng->id);

		glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 10000.0f);
		genericShader.SetMat4("projection", projection);

		glm::mat4 view;
		view = glm::lookAt(glm::vec3(plyr.pos), glm::vec3(plyr.pos + plyr.forward), glm::vec3(VEC_UP));
		genericShader.SetMat4("view", view);

		chunkMan.Render();

		glBindTexture(GL_TEXTURE_2D, 0);
		projection = glm::ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT));

		textShader.Use();
		textShader.SetMat4("projection", projection);

		gui.Label("BobCraft NuDev", Vector(0,0));
		char buf[100];
		snprintf(buf, sizeof(buf), "Position: (%f, %f, %f)", plyr.pos.x, plyr.pos.y, plyr.pos.z);
		gui.Label(buf, Vector(0,1));
		snprintf(buf, sizeof(buf), "Forward: (%f, %f, %f)", plyr.forward.x, plyr.forward.y, plyr.forward.z);
		gui.Label(buf, Vector(0,2));
		snprintf(buf, sizeof(buf), "Angle: (%f, %f)", plyr.pitch, plyr.yaw);
		gui.Label(buf, Vector(0,3));
		snprintf(buf, sizeof(buf), "FPS: %f", 1000 / window.GetSPF());
		gui.Label(buf, Vector(0,4));
		snprintf(buf, sizeof(buf), "Pointed: %i", plyr.pointed.block != nullptr ? plyr.pointed.block->blockType : -1);
		gui.Label(buf, Vector(0,5));
		snprintf(buf, sizeof(buf), "Pointed Position: (%f,%f,%f)", plyr.pointed.position.x, plyr.pointed.position.y, plyr.pointed.position.z);
		gui.Label(buf, Vector(0,6));
		gui.Update();
		
		// TextRendering::RenderText("HELLO WORLD", Vector(0), &texman);

		window.SwapBuffers();
	}
	// Shutdown SDL
	SDL_Quit();

	return 0;
}