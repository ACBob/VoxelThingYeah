// GuiTest - A small test program for the gui system.
// Seperated from the main program to make it easier to debug.

// Does the bare minimum to test the gui system.
// So gets a screen, does the required systems, and then the gui itself.
// Stripped down client main.

#include "shared/logging.hpp"
#include "shared/filesystem.hpp"
#include "shared/localization/localizer.hpp"

#include "rendering/texturemanager.hpp"
#include "rendering/shadermanager.hpp"

// Just use the stubbed out version of the sounds
#ifdef MEEGREEF_ENABLE_OPENAL
#undef MEEGREEF_ENABLE_OPENAL
#endif
#include "sound/soundmanager.hpp"

#include "gui.hpp"

#include "sdlstuff/sdlwindow.hpp"
#include <SDL.h>
#include <SDL_keyboard.h>
#include <SDL_syswm.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main( int argc, char *args[] )
{
	// Because Windows is from the stoneage
#ifdef _WIN32
	SetConsoleOutputCP( CP_UTF8 );
	freopen( "CON", "w", stdout );
	freopen( "CON", "w", stderr );
#endif
	con_info( "Hello from GUI Test!" );

	con_info( "Creating Display..." );
	// Initialize SDL systems
	if ( SDL_Init( SDL_INIT_VIDEO ) )
	{
		char *errBuf = new char[512];
		snprintf( errBuf, 512, "SDL could not initialize!\n SDL_Error: %s", SDL_GetError() );
		con_critical( "ENGINE PANIC: %s", errBuf );
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Engine Panic!", errBuf, NULL );

		delete errBuf;
		return EXIT_FAILURE;
	}
	CGameWindow window( "VoxelThingYeah Baig Test", Vector3f( 800.0f, 600.0f ), true );

	con_info( "Init Filesystem..." );
	if ( !fileSystem::Init( args[0] ) )
	{
		window.Panic( "Couldn't initialise Filesystem! Unrecoverable!" );
	}
	atexit( fileSystem::UnInit );

	con_info( "Mount game files..." );
	if ( !fileSystem::Mount( "files/", "/" ) )
		con_error( "Couldn't mount assets for some reason... nothing will work!" );
	if ( !fileSystem::MountWrite( "files/usr/" ) )
		con_error( "Couldn't set the write directory, your saves are in jeapordy!" );

	CInputManager inputMan;
	window.m_pInputMan = &inputMan;

	con_info( "Now doing all the OpenGL & SDL cruft." );
	{
		atexit( SDL_Quit );
		SDL_GL_LoadLibrary( NULL );

		SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

		SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );

		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );

		SDL_LogSetPriority( SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO );
	}
	window.GatherCTX();
	{
		if ( !gladLoadGLLoader( SDL_GL_GetProcAddress ) )
		{
			window.Panic( "GLAD Could not be loaded!" );
		}
	}

	con_info( "Cool that's done" );
	con_info( "*drumroll* and now... Your feature entertainment... Our internal rendering systems! :)" );

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_FRONT );
	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	glBlendEquation( GL_FUNC_ADD );

	// TODO: Proper loading screen
	glClearColor(0.19f, 0.2f, 0.13f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	window.SwapBuffers();

	shaderSystem::Init();
	atexit( shaderSystem::UnInit );
	materialSystem::Init();
	atexit( materialSystem::UnInit );
	soundSystem::Init();
	atexit( soundSystem::UnInit );

	window.SetIcon( "logo1664.png" );

	con_info( "Init Translator..." );
	// CLocalizer translator;
	// for (std::string &l : translator.ListLanguages() )
	// {
	// 	con_debug( "Found Language: %s", l.c_str() );
	// }

	con_info( "Init GUI..." );

	CGui gui(window.GetSize());
	gui.m_pInputManager = &inputMan;

	inputMan.m_bInGui = true;

	glm::mat4 screen = glm::ortho( 0.0f, 800.0f, 0.0f, 600.0f );

    while ( !window.m_bShouldClose )
	{
		window.PollEvents();
		if ( window.IsFocused() && !inputMan.m_bInGui )
			window.CaptureMouse();
		inputMan.Update();

		if ( window.m_bSizeChanged )
		{
			Vector3f s = window.GetSize();
			glViewport( 0, 0, s.x, s.y );
			screen = glm::ortho( 0.0f, s.x, 0.0f, s.y );
			gui.Resize( s );
			window.m_bSizeChanged = false;
		}

		glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
		// Update screen matrix
		gui.m_pShader->SetMat4( "screen", screen );

		// GUI testing code

		gui.Label("Hello, Baig!", {0, 0});
		gui.Label("Text, but Bigger!", {0, 3}, 2.0f);
		gui.Label("Text, but Smaller!", {0, 7}, 0.5f);

		// End GUI testing code

		gui.Update();
		window.SwapBuffers();
	}

	return EXIT_SUCCESS;
}