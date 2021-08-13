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

#include "physfs.h"

#include <AL/al.h>
#include <AL/alc.h>

#include "sound/soundmanager.h"

void GLAPIENTRY GlMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
				type, severity, message );
}

int main (int argc, char* args[]) {

	// Initialise PhysFS first and foremost
	if (!PHYSFS_init(args[0]))
	{
		printf("PHYSFS Error!\n%s\n", PHYSFS_getLastError());
		return -1;
	}

	if (!PHYSFS_mount("assets", NULL, 0))
	{
		printf("PHYSFS Error when mounting assets!\n%s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return -1;
	}

	// SDL Boilerplate
	{
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

		SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	}

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

	// OpenAL Boilerplate
	ALCdevice *openAlDevice;
	ALCcontext *openAlContext;
	{
		// Now that GL is loaded, do AL
		openAlDevice = alcOpenDevice(NULL);
		if (!openAlDevice)
		{
			printf("OpenAL Initialisation Failed!\n");
			return -1;
		}

		openAlContext = alcCreateContext(openAlDevice, NULL);
		if (!openAlContext)
		{
			printf("OpenAL Intialisation Failed!\n");
			return -1;
		}
		if (!alcMakeContextCurrent(openAlContext))
		{	
			printf("Could not make context current!\n");
			return -1;
		}

		alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
		alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
		float ori[] = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0};
		alListenerfv(AL_ORIENTATION, ori);
		alListenerf(AL_GAIN, 1.0f);
	}

	// Create input manager and give the window a pointer to it
	InputManager input;
	window.inputMan = &input;


	Player plyr = Player();
	plyr.inputMan = &input;

	TextureManager texman;
	Texture* terrainpng = texman.LoadTexture("terrain.png");
	Texture* crosshairpng = texman.LoadTexture("crosshair.png");
	Texture* testpng = texman.LoadTexture("test.png");

	ShaderManager shaderman;
	Shader* genericShader = shaderman.LoadShader("shaders/generic.vert", "shaders/generic.frag");
	Shader* textShader = shaderman.LoadShader("shaders/text.vert", "shaders/text.frag");
	Shader* skyShader = shaderman.LoadShader("shaders/sky.vert", "shaders/sky.frag");

	SoundManager soundMan;
	Sound* testSound = soundMan.LoadSound("sound/test.ogg");
	Sound* breakSound = soundMan.LoadSound("sound/break.ogg");
	Sound* placeSound = soundMan.LoadSound("sound/place.ogg");

	ChunkManager chunkMan(genericShader);

	Model blockHilighter = GetCubeModel(Vector(0.5025, 0.5025, 0.5025));
	blockHilighter.SetShader(genericShader);
	
	Model Skybox = GetCubeModel(Vector(-1, -1, -1));
	Skybox.SetShader(skyShader);

	Model playerVisualHitbox = GetCubeModel(plyr.collision.bounds * -1);
	playerVisualHitbox.SetShader(genericShader);

	GUI gui(&texman, WIDTH, HEIGHT);
	gui.inputMan = &input;
	
	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE); 
	glCullFace(GL_FRONT);

	glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 10000.0f);
	glm::mat4 screen = glm::ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT));

	double time = 0.0;
	const double timeStep = 1/20.0f;
	double currentTime = window.GetTime();

	while(!window.shouldClose) {
		window.PollEvents();

		if (window.IsVisible())
			window.CaptureMouse();

		// Entity handling go here

		plyr.Update(&chunkMan, &soundMan);
		playerVisualHitbox.pos = plyr.collision.pos + Vector(0.5, 1, 0.5);

		double newTime = window.GetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		while (frameTime > 0.0)
		{
			float delta = std::min(frameTime, timeStep);
			plyr.Physics(delta, &chunkMan);
			frameTime -= delta;
			time += delta;
		}

		// OpenAL updating (TODO: THIS BUT BETTER (Make soundMan handle EVERYTHING?))
		{
			alListener3f(AL_POSITION, plyr.camera.pos.x, plyr.camera.pos.y, plyr.camera.pos.z);
			float orient[] = {
				plyr.camera.forward.x,
				plyr.camera.forward.y,
				plyr.camera.forward.z,

				0, 1, 0
			};
			alListenerfv(AL_ORIENTATION, orient);
		}

		// Rendering
		{
			// Rendering right at the end
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			glm::mat4 view = glm::lookAt(glm::vec3(plyr.camera.pos), glm::vec3(plyr.camera.pos + plyr.camera.forward), glm::vec3(VEC_UP));
			shaderman.SetUniforms(view, projection, screen, window.GetMS());

			glDisable(GL_DEPTH_TEST);
			skyShader->Use();
			Skybox.pos = plyr.camera.pos;
			Skybox.Render();
			glEnable(GL_DEPTH_TEST);

			genericShader->Use();

			glBindTexture(GL_TEXTURE_2D, terrainpng->id);

			chunkMan.Render();

			// if (plyr.pointed.block != nullptr && plyr.pointed.block->blockType != blocktype_t::AIR)
			// {
				glLineWidth(4.0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				blockHilighter.pos = plyr.pointed.position - Vector(0.5, 0.5, 0.5);
				blockHilighter.Render();

				playerVisualHitbox.Render();
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			// }

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// GUI
		{
			textShader->Use();

			gui.Label("BobCraft NuDev", Vector(0,0));
			char buf[100];
			snprintf(buf, sizeof(buf), "Position: (%f, %f, %f)", plyr.pos.x, plyr.pos.y, plyr.pos.z);
			gui.Label(buf, Vector(0,1));
			snprintf(buf, sizeof(buf), "Velocity: (%f, %f, %f)", plyr.velocity.x, plyr.velocity.y, plyr.velocity.z);
			gui.Label(buf, Vector(0,2));
			snprintf(buf, sizeof(buf), "Angle: (%f, %f)", plyr.pitch, plyr.yaw);
			gui.Label(buf, Vector(0,3));
			snprintf(buf, sizeof(buf), "FPS: %f", 1000 / window.GetSPF());
			gui.Label(buf, Vector(0,4));
			snprintf(buf, sizeof(buf), "Pointed: %i", plyr.pointed.block != nullptr ? plyr.pointed.block->blockType : -1);
			gui.Label(buf, Vector(0,5));
			snprintf(buf, sizeof(buf), "Pointed Position: (%f,%f,%f)", plyr.pointed.position.x, plyr.pointed.position.y, plyr.pointed.position.z);
			gui.Label(buf, Vector(0,6));
			snprintf(buf, sizeof(buf), "Noclip: %s", plyr.noclipMode ? "Yes" : "No");
			gui.Label(buf, Vector(0,7));

			gui.Image(crosshairpng, gui.screenCentre, Vector(2,2), Vector(0.5,0.5));

			BlockTexture bTex = GetDefaultBlockTextureSide(plyr.selectedBlockType, Direction::NORTH);
			gui.ImageAtlas(terrainpng, {(float)bTex.x, 15.0f - (float)bTex.y, (float)bTex.sizex, (float)bTex.sizey}, 
				16, Vector(gui.screenCentre.x * 2, 0), Vector(8,8), Vector(1.0f,0.0f));

			glDisable(GL_DEPTH_TEST);
			gui.Update();
			glEnable(GL_DEPTH_TEST);
		}

		window.SwapBuffers();
	}
	// Shutdown sound
	alcDestroyContext(openAlContext);
	alcCloseDevice(openAlDevice);

	// Shutdown SDL
	SDL_Quit();

	PHYSFS_deinit();

	return 0;
}