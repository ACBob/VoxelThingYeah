#include "enet/enet.h"

#include "shared/filesystem.hpp"
#include "network/network.hpp"
#include "shared/cvar.hpp"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

#include <stdlib.h>
#include <cstdio>
#include <chrono>

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

#include "gui/gui.hpp"

#include "rendering/modelloader.hpp"

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
	shaderSystem::Init(); atexit(shaderSystem::UnInit);

	Shader* diffuseShader = shaderSystem::LoadShader("shaders/generic.vert", "shaders/generic.frag");
	Shader* skyShader = shaderSystem::LoadShader("shaders/sky.vert", "shaders/sky.frag");
	Shader* unlitShader = shaderSystem::LoadShader("shaders/generic.vert", "shaders/unlit.frag");

	con_info("Load default textures");
	materialSystem::Init(); atexit(materialSystem::UnInit);

	Texture* terrainPng = materialSystem::LoadTexture("terrain.png");
	Texture* crosshairTex = materialSystem::LoadTexture("crosshair.png");
	Texture* testTexture = materialSystem::LoadTexture("test.png");
	Texture* sunTexture = materialSystem::LoadTexture("sun.png");

	modelSystem::Init(); atexit(modelSystem::UnInit);

	con_info("Create Client...");
	World localWorld(diffuseShader, diffuseShader);

	network::Client client;
	client.localWorld = &localWorld;
	if (!client.WorkingClient())
	{
		con_critical("Client ended up in invalid state!");
		return EXIT_FAILURE;
	}

	if (!client.Connect(cl_ip->GetString(), cl_port->GetInt()))
	{
		con_error("Didn't connect to anybody so we've nothing to do!");
		return EXIT_FAILURE;
	}

	EntityPlayer plyr;
	plyr.inputMan = &inputMan;
	plyr.Spawn();
	plyr.SetShader(diffuseShader);
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

	Model skyboxModel;
	GetCubeModel(skyboxModel, Vector(-2,-2,-2));
	skyboxModel.SetShader(skyShader);
	skyboxModel.SetTexture(testTexture);

	Model *skyboxSunModel = modelSystem::LoadModel("models/sun.obj");
	skyboxSunModel->SetTexture(sunTexture);
	skyboxSunModel->SetShader(unlitShader);

	GUI gui(scr_width->GetInt(), scr_height->GetInt());
	gui.inputMan = &inputMan;

	int64_t then = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count();
	int64_t now = then;
	int i = 0;

	bool chatting = false;

	Vector sunForward(0,1,0);
	float sunAngle = 0.0f;
	while (!window.shouldClose)
	{
		client.Update();
		if (!client.connected)
			break;
			
		window.PollEvents();
		if (window.IsFocused() && !chatting)
			window.CaptureMouse();

		if (window.sizeChanged)
		{
			Vector s = window.GetSize();
			scr_width->SetInt(s.x);
			scr_height->SetInt(s.y);
			glViewport(0,0, s.x, s.y);
			gui.Resize(s.x, s.y);
			screen = glm::ortho(0.0f, scr_width->GetFloat(), 0.0f, scr_height->GetFloat());
			window.sizeChanged = false;
		}
		
		inputMan.Update();
		plyr.UpdateClient(client.localWorld);

		if (inputMan.inputState[INKEY_CHAT] && !inputMan.oldInputState[INKEY_CHAT])
		{
			chatting = true;
			inputMan.inGui = true;
		}

		if (inputMan.inputState[INKEY_OUT] && !inputMan.oldInputState[INKEY_OUT])
		{
			if (chatting)
			{
				chatting = false;
				inputMan.inGui = false;
			}
			else // TODO: PAUSE MENU
				window.shouldClose = true;
		}

		// Rendering
		{
			// Rendering right at the end
			glClear(GL_DEPTH_BUFFER_BIT);

			Vector v = plyr.camera.pos + plyr.camera.forward;
			glm::mat4 view = glm::lookAt(glm::vec3(plyr.camera.pos.x, plyr.camera.pos.y, plyr.camera.pos.z), glm::vec3(v.x, v.y, v.z), glm::vec3(VEC_UP.x, VEC_UP.y, VEC_UP.z));
			shaderSystem::SetUniforms(view, projection, screen, window.GetMS(), localWorld.timeOfDay, sunForward);

			glDisable(GL_DEPTH_TEST); // Skybox
			{
				skyboxModel.position = plyr.camera.pos;
				skyboxModel.Render();

				skyboxSunModel->position = skyboxModel.position;
				skyboxSunModel->rotation = Vector(0,0,-sunAngle);
				skyboxSunModel->Render();
			}
			glEnable(GL_DEPTH_TEST);

			diffuseShader->Use();

			glBindTexture(GL_TEXTURE_2D, terrainPng->id);

			localWorld.Render();

			char *buf = new char[100];
			snprintf(buf, 100, "Connected to \"%s\"", cl_servername->GetString());
			gui.Label(buf, Vector(0,0));

			gui.Label(!chatting ? "Bobcraft" : "...", Vector(0,1));

			if (chatting)
			{
				const char *chat = gui.TextInput(0, Vector(0,2));
				if (chat != nullptr)
				{
					protocol::messages::SendClientChatMessage(client.peer, chat);
					chatting = false;
					inputMan.inGui = false;
				}
			}

			for (int i = 0; i < 5; i++)
			{
				int j = client.chatBuffer.size() - i;
				if (j < 0 || j >= client.chatBuffer.size()) continue;
				
				std::string msg = client.chatBuffer.at(j);

				gui.Label(msg.c_str(), Vector(0, 2+i));
			}

			snprintf(buf, 100, "<%.2f,%.2f,%.2f>", plyr.position.x, plyr.position.y, plyr.position.z);
			gui.Label(buf, Vector(0,-1));
			int hours = localWorld.timeOfDay / 1000;
			int minutes = (localWorld.timeOfDay - (hours*1000)) / 16.6666;
			snprintf(buf, 100, "Time %02i:%02i", hours, minutes);
			gui.Label(buf, Vector(0,-2));

			BlockTexture bTex = GetDefaultBlockTextureSide(plyr.selectedBlockType, Direction::NORTH);
			gui.ImageAtlas(terrainPng, {(float)bTex.x, 15.0f - (float)bTex.y, (float)bTex.sizex, (float)bTex.sizey}, 
				16, Vector(-1,-1), Vector(4,4), Vector(1,1));

			gui.Image(crosshairTex, gui.screenCentre, Vector(2,2), Vector(0.5,0.5));
			gui.Update();

			delete[] buf;
		}

		window.SwapBuffers();

		now = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count();
		int64_t delta = now - then;
		if (now >= then) // TICK
		{
			i++;
			
			sunAngle = 180 * (1 - (localWorld.timeOfDay / 12000.0f));
			sunForward = Vector(0,1,0).Rotate(3, sunAngle);

			then = now + 50;
			localWorld.WorldTick(i);

			protocol::messages::SendClientPlayerPos(client.peer, plyr.position, plyr.rotation);
		}
	}
	window.SetVisible(false);

	client.Disconnect();

	return EXIT_SUCCESS;
}