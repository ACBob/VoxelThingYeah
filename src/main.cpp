#include <stdio.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_keyboard.h>

#include "sdlbgfx/sdlwindow.h"

#include "vector.h"
#include "player.h"

#include <fstream>
#include <iostream>

struct PosColorVertex
{
    float x;
    float y;
    float z;
    uint32_t abgr;
};

static PosColorVertex cubeVertices[] =
{
    {-1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeTriList[] =
{
    0, 1, 2,
    1, 3, 2,
    4, 6, 5,
    5, 6, 7,
    0, 2, 4,
    4, 2, 6,
    1, 5, 3,
    5, 7, 3,
    0, 4, 1,
    4, 5, 1,
    2, 3, 6,
    6, 3, 7,
};

// TODO: abstraction yadda yadda
bgfx::ShaderHandle loadShader(const char *filename)
{
	std::ifstream f(filename);
	std::streamsize size = f.tellg();
	f.seekg(0, std::ios::beg);

	char g[size];
	f.read(g, size);

	f.close();

    const bgfx::Memory *mem = bgfx::alloc(size + 1);
    // HACK: tired and want this to work
	for (int i = 0; i < size; i++)
		mem->data[i] = g[i];
    mem->data[mem->size - 1] = '\0';

	return bgfx::createShader(mem);
}

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

	//! TODO: NOT THIS
	bgfx::PlatformData pd;
	SDL_SysWMinfo wmi;

	SDL_VERSION(&wmi.version);
	SDL_GetWindowWMInfo(window.internalWindow.get(), &wmi);

	// TODO: other system support
	#ifdef __linux__
		pd.ndt = wmi.info.x11.display;
	#else
		pd.ndt = NULL;
	#endif
	pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;

	bgfx::setPlatformData(pd);

	bgfx::init();
	bgfx::reset(WIDTH, HEIGHT, BGFX_RESET_VSYNC);
	bgfx::setViewRect(0, 0, 0, uint16_t(WIDTH), uint16_t(HEIGHT));
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x008080ff, 1.0f, 0);
	bgfx::touch(0);

	bgfx::VertexLayout pcvDecl;
    pcvDecl.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
    .end();
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

	bgfx::ShaderHandle fs = loadShader("cube.fs");
	bgfx::ShaderHandle vs = loadShader("cube.vs");
	bgfx::ProgramHandle prg = bgfx::createProgram(vs, fs, true);

	Player plyr = Player();
	plyr.inputMan = &input;

	// Get relative mouse change from MouseMove

	while(!window.shouldClose) {
		window.PollEvents();

		if (window.IsVisible())
			window.CaptureMouse();

		// Entity handling go here

		plyr.Update();

		// Rendering right at the end
        float view[16];
        bx::mtxLookAt(view, plyr.pos + plyr.forward, plyr.pos);
        float proj[16];
        bx::mtxProj(proj, 60.0f, float(WIDTH) / float(HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        bgfx::submit(0, prg);
        bgfx::frame();
	}

	bgfx::shutdown();
	// Shutdown SDL
	SDL_Quit();

	return 0;
}