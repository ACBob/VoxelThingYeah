#include "enet/enet.h"

#include "shared/filesystem.hpp"
#include "network/network.hpp"
#include "shared/cvar.hpp"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

#include <stdlib.h>
#include <cstdio>

#include "utility/assorted.hpp"
#include "cvar_clientside.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_keyboard.h>
#include "sdlstuff/sdlwindow.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "entities/entityplayer.hpp"

int main (int argc, char* args[]) {

	con_info("Hello from bobcraft!");
	con_info("Setting up client-side convars...");
	SetupClientSideConvars();

	char *argstring = FlattenCharArray(args, 1, argc-1);
	con_debug("Args: %s", argstring);
	con_info("Parsing command line convars...");
	conVarHandle.Parse(argstring);

	con_info("Init Filesystem...");
	if (!fileSystem::Init(args[0]))
	{
		con_critical("Couldn't initialise Filesystem! Unrecoverable!");
		return EXIT_FAILURE;
	}
	atexit(fileSystem::UnInit);

	con_info("Mount assets...");
	if (!fileSystem::Mount("assets/", "/"))
		con_error("Couldn't mount assets for some reason... nothing will work!");

	con_info("Init Network...");
	if (!network::Init())
	{
		con_critical("Couldn't initialise Network! Unrecoverable!");
		return EXIT_FAILURE;
	}
	atexit(network::Uninit);

	con_info("Now doing all the OpenGL & SDL cruft.");
	{
		// Initialize SDL systems
		if(SDL_Init( SDL_INIT_VIDEO )) {
			con_critical("SDL could not initialize!\n SDL_Error: %s\n",
						SDL_GetError());
			return EXIT_FAILURE;
		}
		atexit(SDL_Quit);
		SDL_GL_LoadLibrary(NULL);

		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

		SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	}
	GameWindow window("VoxelThingYeah", Vector(scr_width->GetFloat(),scr_height->GetFloat()), true);
	InputManager inputMan;
	window.inputMan = &inputMan;
	{
		if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
		{
			// TODO: Use SDL rendering and display an error on-screen
			con_critical("Cannot load Glad\n");
			return EXIT_FAILURE;
		}
	}

	con_info("Cool that's done");

	con_info("*drumroll* and now... Your feature entertainment... Our internal rendering systems! :)");
	con_info("Load default shaders");
	ShaderManager shaderMan;
	Shader* worldShader = shaderMan.LoadShader("shaders/generic.vert", "shaders/generic.frag");
	Shader* skyShader = shaderMan.LoadShader("shaders/sky.vert", "shaders/sky.frag");
	Shader* entityShader = worldShader;

	con_info("Load default textures");
	TextureManager texMan;
	Texture* terrainPng = texMan.LoadTexture("terrain.png");

	con_info("Create Client...");
	World localWorld(worldShader, entityShader);

	network::Client client;
	client.localWorld = &localWorld;
	if (!client.WorkingClient())
	{
		con_critical("Client ended up in invalid state!");
		return EXIT_FAILURE;
	}

	if (!client.Connect())
	{
		con_error("Didn't connect to anybody so we've nothing to do!");
		return EXIT_FAILURE;
	}

	EntityPlayer plyr;
	plyr.inputMan = &inputMan;
	plyr.Spawn();
	plyr.SetShader(worldShader);
	client.localPlayer = &plyr;
	localWorld.AddEntity(&plyr);
	plyr.client = &client;

	glm::mat4 projection = glm::perspective(glm::radians(fov->GetFloat()), scr_width->GetFloat() / scr_height->GetFloat(), 0.1f, 10000.0f);
	glm::mat4 screen = glm::ortho(0.0f, scr_width->GetFloat(), 0.0f, scr_height->GetFloat());

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE); 
	glCullFace(GL_FRONT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,  GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	Model skyboxModel = GetCubeModel(Vector(-1,-1,-1));
	skyboxModel.SetShader(skyShader);

	while (!window.shouldClose)
	{
		client.Update();
		if (!client.connected)
			break;
			
		window.PollEvents();
		if (window.IsFocused())
			window.CaptureMouse();
		
		plyr.UpdateClient(client.localWorld);

		localWorld.WorldTick(0);

		// Rendering
		{
			// Rendering right at the end
			glClear(GL_DEPTH_BUFFER_BIT);

			Vector v = plyr.camera.pos + plyr.camera.forward;
			glm::mat4 view = glm::lookAt(glm::vec3(plyr.camera.pos.x, plyr.camera.pos.y, plyr.camera.pos.z), glm::vec3(v.x, v.y, v.z), glm::vec3(VEC_UP.x, VEC_UP.y, VEC_UP.z));
			shaderMan.SetUniforms(view, projection, screen, window.GetMS());

			glDisable(GL_DEPTH_TEST); // Skybox
			{
				skyboxModel.pos = plyr.camera.pos;
				skyboxModel.Render();
			}
			glEnable(GL_DEPTH_TEST);

			worldShader->Use();

			glBindTexture(GL_TEXTURE_2D, terrainPng->id);

			for (void *ent : localWorld.ents)
			{
				((EntityBase*)ent)->Render();
			}

			localWorld.Render();
		}

		window.SwapBuffers();
	}
	window.SetVisible(false);

	client.Disconnect();

	return EXIT_SUCCESS;
}