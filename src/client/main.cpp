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
#include "gui/guistatemanager.hpp"

#include "rendering/modelloader.hpp"

#include "sound/soundmanager.hpp"

#include "particles/particlesystem.hpp"

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
	con_info( "Load default shaders" );
	shaderSystem::Init();
	atexit( shaderSystem::UnInit );

	CShader *diffuseShader	   = shaderSystem::LoadShader( "shaders/generic.vert", "shaders/generic.frag" );
	CShader *skyShader		   = shaderSystem::LoadShader( "shaders/sky.vert", "shaders/sky.frag" );
	CShader *unlitShader	   = shaderSystem::LoadShader( "shaders/generic.vert", "shaders/unlit.frag" );
	CShader *viewDiffuseShader = shaderSystem::LoadShader( "shaders/viewmodel.vert", "shaders/generic.frag" );

	con_info( "Load default textures" );
	materialSystem::Init();
	atexit( materialSystem::UnInit );

	CTexture *terrainPng   = materialSystem::LoadTexture( "terrain.png" );
	CTexture *crosshairTex = materialSystem::LoadTexture( "crosshair.png" );
	CTexture *testTexture  = materialSystem::LoadTexture( "test.png" );
	CTexture *sunTexture   = materialSystem::LoadTexture( "sun.png" );
	CTexture *playerTex = nullptr;
	
	if (playerskin->IsModified())
		playerTex = materialSystem::LoadTexture( playerskin->GetString() );

	con_info( "Loading some models" );
	modelSystem::Init();
	atexit( modelSystem::UnInit );

	CModel skyboxModel;
	GetCubeModel( skyboxModel, CVector( -2, -2, -2 ) );
	skyboxModel.SetShader( skyShader );
	skyboxModel.SetTexture( testTexture );

	CModel *skyboxSunModel = modelSystem::LoadModel( "models/sun.obj" );
	skyboxSunModel->SetTexture( sunTexture );
	skyboxSunModel->SetShader( unlitShader );

	CModel a;
	GetCubeModel( a, CVector( 0.2, 0.2, 0.2 ) );
	a.SetShader( unlitShader );
	a.SetTexture( testTexture );

	CModel viewModel;
	GetCubeModel( viewModel, CVector( 0.3, 0.3, 0.3 ), CVector( 0.0f, 0.0f, 1 / 16.0f, 1 / 16.0f ) );
	viewModel.SetTexture( terrainPng );
	viewModel.SetShader( viewDiffuseShader );
	viewModel.m_vPosition = CVector( 0.4, -0.4, -0.4 );
	viewModel.m_vRotation = CVector( 0, 45, 0 );

	con_info( "Loading Sounds..." );
	soundSystem::Init();
	atexit( soundSystem::UnInit );

	con_info( "Init ParticleSystem..." );
	particleSystem::Init();
	atexit( particleSystem::UnInit );

	con_info( "Create Client..." );
	CWorld localWorld( diffuseShader, diffuseShader );

	CNetworkClient client;
	client.m_pLocalWorld = &localWorld;
	if ( !client.WorkingClient() )
	{
		con_critical( "Client ended up in invalid state!" );
		return EXIT_FAILURE;
	}

	if ( !client.Connect( cl_ip->GetString(), cl_port->GetInt() ) )
	{
		con_error( "Didn't connect to anybody so we've nothing to do!" );
		return EXIT_FAILURE;
	}

	CEntityPlayer plyr;
	plyr.m_pInputMan = &inputMan;
	plyr.Spawn();
	plyr.SetShader( diffuseShader );
	client.m_pLocalPlayer = &plyr;
	localWorld.AddEntity( &plyr );
	localWorld.m_pLocalPlayer = &plyr;
	plyr.m_pClient			  = &client;

	if (playerTex != nullptr)
	{
		plyr.m_pMdl->SetTexture(playerTex);
		con_info("Sending our skin");
		protocol::SendClientSkin( client.m_pPeer, plyr.m_pMdl->GetTexture()->m_imageData, 8 );
	}

	glm::mat4 projection = glm::perspective( glm::radians( fov->GetFloat() ),
											 scr_width->GetFloat() / scr_height->GetFloat(), 0.1f, 10000.0f );
	glm::mat4 screen	 = glm::ortho( 0.0f, scr_width->GetFloat(), 0.0f, scr_height->GetFloat() );
	glm::mat4 viewScreen =
		glm::lookAt( glm::vec3( 0, 0, 0 ), glm::vec3( 0, 0, -1 ), glm::vec3( VEC_UP.x, VEC_UP.y, VEC_UP.z ) );

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_FRONT );
	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	glBlendEquation( GL_FUNC_ADD );

	CGui gui( scr_width->GetInt(), scr_height->GetInt() );
	gui.m_pInputMan = &inputMan;
	gui.m_pClient	= &client;
	CGuiStateManager guiState;
	guiState.m_pInputManager = &inputMan;
	guiState.m_pGui			 = &gui;

	int64_t then =
		std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() )
			.count();
	int64_t now = then;
	int i		= 0;

	CParticle testParticle;
	testParticle.m_mdl		 = particleSystem::particleMdl;
	testParticle.m_vPosition = CVector( 8, 8, 8 );

	CVector sunForward( 0, 1, 0 );
	float sunAngle = 0.0f;
	while ( !window.m_bShouldClose )
	{
		client.Update();
		if ( !client.m_bConnected )
			break;

		window.PollEvents();
		if ( window.IsFocused() && !inputMan.m_bInGui )
			window.CaptureMouse();

		if ( window.m_bSizeChanged )
		{
			CVector s = window.GetSize();
			scr_width->SetInt( s.x );
			scr_height->SetInt( s.y );
			glViewport( 0, 0, s.x, s.y );
			gui.Resize( s.x, s.y );
			projection			  = glm::perspective( glm::radians( fov->GetFloat() ),
											  scr_width->GetFloat() / scr_height->GetFloat(), 0.1f, 10000.0f );
			screen				  = glm::ortho( 0.0f, scr_width->GetFloat(), 0.0f, scr_height->GetFloat() );
			window.m_bSizeChanged = false;
		}

		inputMan.Update();
		plyr.UpdateClient( client.m_pLocalWorld );

		soundSystem::SetListener( &localWorld, plyr.m_camera.m_vPosition, plyr.m_camera.GetForward(),
								  plyr.m_vVelocity );

		// Rendering
		{
			// Rendering right at the end
			glClear( GL_DEPTH_BUFFER_BIT );

			CVector f			= plyr.m_camera.GetForward();
			CVector v			= plyr.m_camera.m_vPosition + f;
			glm::mat4 viewWorld = glm::lookAt(
				glm::vec3( plyr.m_camera.m_vPosition.x, plyr.m_camera.m_vPosition.y, plyr.m_camera.m_vPosition.z ),
				glm::vec3( v.x, v.y, v.z ), glm::vec3( VEC_UP.x, VEC_UP.y, VEC_UP.z ) );
			shaderSystem::SetUniforms( viewWorld, viewScreen, projection, screen, window.GetMS(),
									   localWorld.m_iTimeOfDay, sunForward );

			glDisable( GL_DEPTH_TEST ); // Skybox
			{
				skyboxModel.m_vPosition = plyr.m_camera.m_vPosition;
				skyboxModel.Render();

				skyboxSunModel->m_vPosition = skyboxModel.m_vPosition;
				skyboxSunModel->m_vRotation = CVector( 0, 0, -sunAngle );
				skyboxSunModel->Render();
			}
			glEnable( GL_DEPTH_TEST );

			diffuseShader->Use();

			glBindTexture( GL_TEXTURE_2D, terrainPng->m_iId );

			localWorld.Render();

			// for (CParticleEmitter pe : particleSystem::particleEmitters)
			// {
			// 	pe.Render();
			// }

			// testParticle.m_vPosition = v;
			// testParticle.Render();
			// a.m_vPosition = testParticle.m_vPosition;
			// a.Render();

			guiState.Update( &localWorld );

			BlockTexture bTex	  = GetDefaultBlockTextureSide( plyr.m_iSelectedBlockType, Direction::NORTH );
			viewModel.m_vUvOffset = CVector( bTex.x, bTex.y ) * ( 1 / 16.0f );
			viewModel.Render();
		}

		window.SwapBuffers();

		now =
			std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() )
				.count();
		int64_t delta = now - then;
		if ( now >= then ) // TICK
		{
			i++;

			sunAngle   = 180 * ( 1 - ( localWorld.m_iTimeOfDay / 12000.0f ) );
			sunForward = CVector( 0, 1, 0 ).Rotate( 3, sunAngle );

			then = now + 50;
			localWorld.WorldTick( i );

			protocol::SendClientPlayerPos( client.m_pPeer, plyr.m_vPosition, plyr.m_vRotation );
		}
	}
	window.SetVisible( false );

	client.Disconnect();

	return EXIT_SUCCESS;
}