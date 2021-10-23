#include "enet/enet.h"

#include "network/client.hpp"
#include "network/network.hpp"
#include "shared/cvar.hpp"
#include "shared/filesystem.hpp"

#include "shared/logging.hpp"

#include <chrono>
#include <cstdio>
#include <stdlib.h>

#include "cvar_clientside.hpp"
#include "utility/assorted.hpp"

#include "sdlstuff/sdlwindow.hpp"
#include <SDL.h>
#include <SDL_keyboard.h>
#include <SDL_syswm.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "entities/entityplayer.hpp"

#include "gui/gui.hpp"

#include "rendering/modelloader.hpp"

#include "sound/soundmanager.hpp"

#include "particles/particlesystem.hpp"

#include "gamestates.hpp"

#include "packs.hpp"

#include "devconsole.hpp"

#ifdef _WIN32
	#include <stdio.h>
	#include <windows.h>
#endif

int main( int argc, char *args[] )
{
	// Because Windows is from the stoneage
#ifdef _WIN32
	SetConsoleOutputCP( CP_UTF8 );
	freopen( "CON", "w", stdout );
	freopen( "CON", "w", stderr );
#endif
	con_info( "Hello from Meegreef!" );
	con_info( "Setting up client-side convars..." );
	SetupClientSideConvars();

	char *argstring = FlattenCharArray( args, 1, argc - 1 );
	con_debug( "Args: %s", argstring );
	con_info( "Parsing command line convars..." );
	conVarHandle.Parse( argstring );

	// seed random
	srand( time( NULL ) );

	con_debug( "Your random number today is %d", rand() % 500 );

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
	CGameWindow window( "VoxelThingYeah", CVector( scr_width->GetFloat(), scr_height->GetFloat() ), true );

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

	con_info( "Parsing config.cfg..." );
	bool succeed;
	int64_t l;
	char *file = (char *)fileSystem::LoadFile( "usr/config.cfg", l, succeed );
	if ( succeed )
		conVarHandle.Parse( file );
	
	con_info("Scan for resource pack...");
	if (cl_resourcepacks->IsModified())
	{
		size_t n = strlen(cl_resourcepacks->GetString());
		char *l = new char[n + 1]();
		strcpy(l, cl_resourcepacks->GetString());


		// test each pack
		char *t;
		char *saveptr;
		t = strtok_r( l, ",", &saveptr );

		while ( t != NULL )
		{
			resourcePacks::packInfo inf = resourcePacks::GetPackInfo(t);

			if (inf.format == 0x00)
			{
				con_warning("Unknown Pack %s", t);
			}
			else
			{
				con_info("%s (%s)", inf.name.c_str(), inf.path.c_str());

				resourcePacks::MountPack(inf);
			}

			t = strtok_r( NULL, ",", &saveptr );
		}
	}

	con_info( "Init Network..." );
	if ( !network::Init() )
	{
		window.Panic( "Couldn't initialise Network! Unrecoverable!" );
	}
	atexit( network::Uninit );

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
	modelSystem::Init();
	atexit( modelSystem::UnInit );
	soundSystem::Init();
	atexit( soundSystem::UnInit );

	window.SetIcon( "logo1664.png" );

	con_info( "Init ParticleSystem..." );
	particleSystem::Init();
	atexit( particleSystem::UnInit );

	con_info( "Create Client..." );
	CNetworkClient client;
	if ( !client.WorkingClient() )
	{
		window.Panic( "Client ended up in invalid state!" );
	}

	con_info( "Init GUI..." );

	CGui gui( scr_width->GetInt(), scr_height->GetInt() );
	gui.m_pInputMan = &inputMan;
	gui.m_pClient	= &client;
	gui.m_iGuiUnit	= floor( window.GetSize().x / 53 );
	gui.Resize( scr_width->GetInt(), scr_height->GetInt() );

	con_info( "Init Dev-Console..." );
	CDevConsole console;
	console.m_pGui = &gui;
	console.m_pInputMan = &inputMan;

	con_info( "Init Game State..." );
	CGameStateMachine gameStateMan;
	gameStateMan.m_pClient	 = &client;
	gameStateMan.m_pGui		 = &gui;
	gameStateMan.m_pWindow	 = &window;
	gameStateMan.m_pInputMan = &inputMan;
	gameStateMan.PushState( std::make_unique<CStateMenu>() );

	inputMan.m_bInGui = true;

	glm::mat4 screen = glm::ortho( 0.0f, scr_width->GetFloat(), 0.0f, scr_height->GetFloat() );
	glm::mat4 a		 = glm::ortho( 0.0f, 0.0f, 0.0f, 0.0f );
	glm::mat4 b		 = glm::ortho( 0.0f, 0.0f, 0.0f, 0.0f );

	int64_t now =
		std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() )
			.count();
	int64_t then	 = now;
	int64_t nextTick = now;

	int64_t delta = 0;

	int64_t musicTick = 0;

	while ( !window.m_bShouldClose )
	{
		window.PollEvents();
		if ( window.IsFocused() && !inputMan.m_bInGui )
			window.CaptureMouse();
		inputMan.Update();

		if ( window.m_bSizeChanged )
		{
			CVector s = window.GetSize();
			scr_width->SetInt( s.x );
			scr_height->SetInt( s.y );
			glViewport( 0, 0, s.x, s.y );
			screen = glm::ortho( 0.0f, scr_width->GetFloat(), 0.0f, scr_height->GetFloat() );
			gui.Resize( s.x, s.y );
			window.m_bSizeChanged = false;
		}

		glClear( GL_DEPTH_BUFFER_BIT );
		// Update screen matrix
		for ( CShader *shd : shaderSystem::loadedShaders )
		{
			shd->SetMat4( "screen", screen );
		}

		if ( gameStateMan.m_iTick >= musicTick )
		{
			musicTick = gameStateMan.m_iTick + 20000;

			if ( rand() % 100 > 50 )
				soundSystem::PlaySoundEvent( "game.music", CVector( 0, 0, 0 ) );
		}

		gameStateMan.Update();
		
		// Update dev console after this to remain on-top
		console.Update();

		gui.m_iTick = gameStateMan.m_iTick;
		gui.Update();
		client.Update();

		window.SwapBuffers();

		now =
			std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() )
				.count();
		delta = now - then;
		then  = now;
		if ( now >= nextTick )
		{
			nextTick = now + 50;
			gameStateMan.m_iTick++;
		}

		gameStateMan.m_fDelta = delta / 1000.0f;

		if ( gameStateMan.IsEmpty() )
			break;
	}
	gameStateMan.Flush();

	conVarHandle.WriteCFG();

	return EXIT_SUCCESS;
}