#include "logging.hpp"

#include "sdlstuff/sdlwindow.hpp"

#ifdef _WIN32
	#include <stdio.h>
#endif

#include "cvar_clientside.hpp"

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "shared/filesystem.hpp"
#include "shared/network/network.hpp"

int main( int argc, char *args[] )
{
	// Because Windows is from the stoneage
#ifdef _WIN32
	SetConsoleOutputCP( CP_UTF8 );
	freopen( "CON", "w", stdout );
	freopen( "CON", "w", stderr );
#endif

    con_info("Hello VoxelThingYeah!");
    con_info("Setup Convars...");

    SetupClientSideConvars();

    // flatten args to a single string
    char *args_str = new char[(argc - 1) * 1024];
    args_str[0] = '\0';
    for (int i = 1; i < argc - 1; i++)
    {
        strcat(args_str, args[i]);
        strcat(args_str, " ");
    }
    conVarHandle.Parse(args_str);
    delete[] args_str;

    // Seed Random
    srand(time(NULL));

    con_info("Creating Display...");
    if ( SDL_Init( SDL_INIT_VIDEO ) )
	{
		char *errBuf = new char[512];
		snprintf( errBuf, 512, "SDL could not initialize!\n SDL_Error: %s", SDL_GetError() );
		con_critical( "ENGINE PANIC: %s", errBuf );
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Engine Panic!", errBuf, NULL );

		delete[] errBuf;
		return EXIT_FAILURE;
	}

    CGameWindow window( "VoxelThingYeah", 1280, 720, false );

    con_info("Init Filesystem...");
    if (!fileSystem::Init( args[0] ))
        window.Panic("Failed to initialize filesystem!");
    atexit(fileSystem::UnInit);

    con_info("Mount Game Data...");
    if (!fileSystem::Mount("files/", "/"))
        con_critical("Failed to mount game data!");
    if (!fileSystem::MountWrite("files/usr/"))
        con_critical("Failed to mount user directory!");
    
    // TODO: parse config.cfg

    con_info("Init Network...");
    if (!network::Init())
        window.Panic("Failed to initialize network!"); // TODO: Single-player only mode?
    atexit(network::Uninit);

    CInputManager inputManager;
    window.m_pInputMan = &inputManager;

    con_info("Doing OpenGL/SDL cruft...");
    {
		atexit( SDL_Quit );
		SDL_GL_LoadLibrary( NULL );

		SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 ); // TODO: OpenGL 3.3 support
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

		SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );

		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );

		SDL_LogSetPriority( SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO );

        window.GatherCTX();

        if ( !gladLoadGLLoader( SDL_GL_GetProcAddress ) )
		{
			window.Panic( "GLAD Could not be loaded!" );
		}

        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LEQUAL );
        glEnable( GL_CULL_FACE );
        glCullFace( GL_BACK );
        glEnable( GL_BLEND );
        glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
        glBlendEquation( GL_FUNC_ADD );
    }

    con_info("Ok cool done");

    window.SetIcon( "logo64.png" );


    return 0;
}