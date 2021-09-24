#include "enet/enet.h"

#include "network/client.hpp"
#include "network/network.hpp"
#include "shared/cvar.hpp"
#include "shared/filesystem.hpp"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

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

int main( int argc, char *args[] )
{

	con_info( "Hello from bobcraft!" );
	con_info( "Setting up client-side convars..." );
	SetupClientSideConvars();

	char *argstring = FlattenCharArray( args, 1, argc - 1 );
	con_debug( "Args: %s", argstring );
	con_info( "Parsing command line convars..." );
	conVarHandle.Parse( argstring );

	con_info( "Init Filesystem..." );
	if ( !fileSystem::Init( args[0] ) )
	{
		con_critical( "Couldn't initialise Filesystem! Unrecoverable!" );
		return EXIT_FAILURE;
	}
	atexit( fileSystem::UnInit );

	con_info( "Mount assets..." );
	if ( !fileSystem::Mount( "assets/", "/" ) )
		con_error( "Couldn't mount assets for some reason... nothing will work!" );

	con_info( "Init Network..." );
	if ( !network::Init() )
	{
		con_critical( "Couldn't initialise Network! Unrecoverable!" );
		return EXIT_FAILURE;
	}
	atexit( network::Uninit );

	con_info( "Now doing all the OpenGL & SDL cruft." );
	{
		// Initialize SDL systems
		if ( SDL_Init( SDL_INIT_VIDEO ) )
		{
			con_critical( "SDL could not initialize!\n SDL_Error: %s\n", SDL_GetError() );
			return EXIT_FAILURE;
		}
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
	CGameWindow window( "VoxelThingYeah", CVector( scr_width->GetFloat(), scr_height->GetFloat() ), true );
	CInputManager inputMan;
	window.m_pInputMan = &inputMan;
	{
		if ( !gladLoadGLLoader( SDL_GL_GetProcAddress ) )
		{
			// TODO: Use SDL rendering and display an error on-screen
			con_critical( "Cannot load Glad\n" );
			return EXIT_FAILURE;
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

	shaderSystem::Init();
	atexit( shaderSystem::UnInit );
	materialSystem::Init();
	atexit( materialSystem::UnInit );
	modelSystem::Init();
	atexit( modelSystem::UnInit );
	soundSystem::Init();
	atexit( soundSystem::UnInit );

	con_info( "Init ParticleSystem..." );
	particleSystem::Init();
	atexit( particleSystem::UnInit );

	con_info( "Create Client..." );
	CNetworkClient client;
	if ( !client.WorkingClient() )
	{
		con_critical( "Client ended up in invalid state!" );
		return EXIT_FAILURE;
	}

	con_info( "Init GUI..." );

	CGui gui( scr_width->GetInt(), scr_height->GetInt() );
	gui.m_pInputMan = &inputMan;
	gui.m_pClient	= &client;
	gui.m_iGuiUnit	= floor( window.GetSize().x / 53 );
	gui.Resize( scr_width->GetInt(), scr_height->GetInt() );

	con_info( "Init Game State..." );
	CGameStateMachine gameStateMan;
	gameStateMan.PushState( std::make_unique<CStateMenu>() );
	gameStateMan.m_pClient	 = &client;
	gameStateMan.m_pGui		 = &gui;
	gameStateMan.m_pWindow	 = &window;
	gameStateMan.m_pInputMan = &inputMan;

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

		gameStateMan.Update();
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
	}
	gameStateMan.Flush();

	return EXIT_SUCCESS;
}