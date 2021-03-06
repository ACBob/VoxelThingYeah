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
#include "shared/cvar_shared.hpp"
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

#include <thread>

#include "devconsole.hpp"

#ifdef __linux__
	#include <pwd.h>
	#include <unistd.h>
#endif

#include "localization/localizer.hpp"

#ifdef _WIN32
	#include <stdio.h>
	#include <windows.h>
	#include <lmcons.h>
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
	con_info( "Setting up convars..." );
	SetupClientSideConvars();
	SetupSharedConvars();

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

		delete[] errBuf;
		return EXIT_FAILURE;
	}
	CGameWindow window( "VoxelThingYeah", Vector3f( scr_width->GetFloat(), scr_height->GetFloat() ), true );

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

	if ( !username->IsModified() )
	{
		// Get the user's computer username to use instead
#ifdef __linux__
		// The linux way!
		passwd *pw;
		uid_t uid;

		uid = geteuid();
		pw	= getpwuid( uid );

		if ( pw ) {
			// Truncate the username to 32 characters
			// (TODO: what even is the max length of a linux username?)
			char *buf = new char[32];
			strncpy( buf, pw->pw_name, 32 );
			username->SetString( buf );
			delete[] buf;
		}
		else // In the situation we can't find it, just use player
			username->SetString( "Player" );
#elif _WIN32

		// I have no idea what GetUserNameA will return, nor do I know what GetUserNameExA with NameDisplay will return.
		// I hate windows.
		// I can't even use GetUserNameExA, because it's in secext.h, and it wants me to define something, and I don't know what that does.
		// Here's hoping GetUserNameA will work.
		char *name = new char[UNLEN + 1];
		DWORD size = UNLEN + 1;
		if (GetUserNameA( name, &size ) == 0) {
			// Of-course who the hell knows how long UNLEN is?
			// Just truncate to 32 characters.
			name[32] = '\0';
			username->SetString( name );
		}
		else
			username->SetString( "WinPlayer" );
		delete[] name;

#endif

		con_warning( "Username set to %s", username->GetString() );
	}

	con_info( "Scan for resource pack..." );
	if ( cl_resourcepacks->IsModified() )
	{
		size_t n = strlen( cl_resourcepacks->GetString() );
		char *l	 = new char[n + 1]();
		strcpy( l, cl_resourcepacks->GetString() );

		// test each pack
		char *t;
		char *saveptr;
		t = strtok_r( l, ",", &saveptr );

		while ( t != NULL )
		{
			resourcePacks::packInfo inf = resourcePacks::GetPackInfo( t );

			if ( inf.format == 0x00 )
			{
				con_warning( "Unknown Pack %s", t );
			}
			else
			{
				con_info( "%s (%s)", inf.name.c_str(), inf.path.c_str() );

				resourcePacks::MountPack( inf );
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
	glCullFace( GL_BACK );
	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	glBlendEquation( GL_FUNC_ADD );

	// TODO: Proper loading screen
	glClearColor( 0.19f, 0.2f, 0.13f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

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

	con_info( "Init Translator..." );
	CLocalizer translator;

	client.m_pTranslator = &translator;

	con_info( "Init GUI..." );

	CGui gui( window.GetSize() );
	gui.m_pInputManager = &inputMan;

	con_info( "Init Dev-Console..." );
	CDevConsole console;
	console.m_pGui		= &gui;
	console.m_pInputMan = &inputMan;

	con_info( "Init Game State..." );
	CGameStateMachine gameStateMan;
	gameStateMan.m_pClient	  = &client;
	gameStateMan.m_pGui		  = &gui;
	gameStateMan.m_pWindow	  = &window;
	gameStateMan.m_pInputMan  = &inputMan;
	gameStateMan.m_pLocalizer = &translator;
	gameStateMan.PushState( std::make_unique<CStateMenu>() );

	inputMan.m_bInGui = true;

	glm::mat4 screen = glm::ortho( 0.0f, scr_width->GetFloat(), scr_height->GetFloat(), 0.0f );

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
			Vector3f s = window.GetSize();
			scr_width->SetInt( s.x );
			scr_height->SetInt( s.y );
			glViewport( 0, 0, s.x, s.y );
			screen = glm::ortho( 0.0f, s.x, s.y, 0.0f );
			gui.Resize( s );
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
				soundSystem::PlaySoundEvent( "game.music", Vector3f( 0, 0, 0 ) );
		}

		gameStateMan.Update();

		// Update dev console after this to remain on-top
		console.Update();

		gui.m_iTick = gameStateMan.m_iTick;
		gui.Update();
		client.Update();

		window.SwapBuffers();

		// Screenshotting
		if ( inputMan.m_bKeyboardState[KBD_F2] && !inputMan.m_bOldKeyboardState[KBD_F2] )
		{
			// TODO: Abstract this
			int w = scr_width->GetInt();
			int h = scr_height->GetInt();

			std::vector<unsigned char> pixels( w * h * 4 );
			glReadPixels( 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data() );

			// But we need to flip it vertically
			int lineSize = w * 4;
			for ( int y = 0; y < h / 2; y++ )
			{
				for ( int x = 0; x < lineSize; x++ )
				{
					std::swap( pixels[y * lineSize + x], pixels[(h - y - 1) * lineSize + x] );
				}
			}

			// Make sure /screenshots/ exists
			fileSystem::CreateDirectory( "/screenshots/" );

			// Plop this into a texture (material system expects a std::vector)
			CTexture *pTex = materialSystem::LoadTexture( pixels, w, h );
			// Save the texture to disk
			char *filename = new char[256];
			// Take the current time and make a filename
			time_t t = time( nullptr );
			strftime( filename, 256, "/screenshots/screenshot_%Y-%m-%d_%H-%M-%S.png", localtime( &t ) );
			pTex->Save( filename );

			client.m_chatBuffer.push_back( (std::string)"Screenshot saved to " + filename );

			// Clean up
			delete[] filename;
			delete pTex;
		}

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

		// use the cl_maxfps cvar to cap the framerate
		if ( cl_maxfps->GetInt() > 0 )
		{
			int64_t maxFrameTime = 1000 / cl_maxfps->GetInt();
			int64_t sleepTime	 = maxFrameTime - delta;
			if ( sleepTime > 0 )
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( sleepTime ) );
			}
		}

		if ( gameStateMan.IsEmpty() )
			break;
	}
	gameStateMan.Flush();

	conVarHandle.WriteCFG( "config.cfg" );

	return EXIT_SUCCESS;
}