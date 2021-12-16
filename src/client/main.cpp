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

#include "rendering/model.hpp"
#include "rendering/material.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#include "gui/shtoigui.hpp"

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

    CGameWindow window( "VoxelThingYeah", Vector3i( scr_width->GetInt(), scr_height->GetInt() ), false );

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

    con_info("Init Renderer...");
    if (!materialSystem::Init())
        window.Panic("Failed to initialize material system!");
    atexit(materialSystem::Uninit);

    CModel testModel;
    testModel.LoadOBJ( "player.obj" );

    glm::mat4 projection = glm::perspective( glm::radians( fov->GetFloat() ),
        scr_width->GetFloat() / scr_height->GetFloat(), 0.1f, 10000.0f );
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 screen = glm::ortho( 0.0f, scr_width->GetFloat(), scr_height->GetFloat(), 0.0f, -1.0f, 1.0f );

    Vector3f camPos = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f camLook = Vector3f(0.0f, 0.0f, -1.0f);
    Vector3f camUp = Vector3f(0.0f, 1.0f, 0.0f);

    materialSystem::CShader *guiShader = materialSystem::GetNamedShader( "gui" );
    materialSystem::CTexture *testTexture = materialSystem::LoadTexture( "filledSquare.png" );
    
    float camPitch = 0.0f;
    float camYaw = 0.0f;

    CShtoiGUI gui( ShtoiGUI_displayMode::TwoDee, window.GetSize().x, window.GetSize().y );

    while ( !window.m_bShouldClose )
    {
        window.PollEvents();
        inputManager.Update();

        glClearColor( 0.0f, 0.5f, 0.5f, 1.0f );
        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

        // camera rotation
        camPitch += inputManager.m_vMouseMovement.y * 0.1f;
        camYaw += inputManager.m_vMouseMovement.x * 0.1f;

        if ( inputManager.m_bInputState[CONTROLS_FRONT] )
            camPos += camLook * 0.1f;
        if ( inputManager.m_bInputState[CONTROLS_BACK] )
            camPos -= camLook * 0.1f;
        if ( inputManager.m_bInputState[CONTROLS_LEFT] )
            camPos -= camLook.Cross( camUp ).Normal() * 0.1f;
        if ( inputManager.m_bInputState[CONTROLS_RIGHT] )
            camPos += camLook.Cross( camUp ).Normal() * 0.1f;

        camPitch = fminf( fmaxf( camPitch, -89.9f ), 89.9f );

        camLook = Vector3f(0, 0, -1.0f).RotateAxis( 0, -camPitch * DEG2RAD ).RotateAxis( 1, -camYaw * DEG2RAD );

        view = glm::lookAt(glm::vec3(camPos.x, camPos.y, camPos.z), glm::vec3(camPos.x + camLook.x, camPos.y + camLook.y, camPos.z + camLook.z), glm::vec3(camUp.x, camUp.y, camUp.z));

        materialSystem::UpdateUniforms( projection, view, screen );

        testModel.Render(
            Vector3f( 0.0f, 0.0f, 1.0f ),
            Vector3f( 0.0f, 0.0f, 0.0f ),
            Vector3f( 1.0f, 1.0f, 1.0f )
        );
        
        // gui
        gui.Rect( 0, 0, 0, 200, 200, 0, 0, 0, 1 );

        gui.BeginLayout( ShtoiGUI_layoutType::FlexRows, 0, 0, 0, 200, 200, 15, 15, 4);
        {
            gui.SetLayoutNumbers(4, 2);
            gui.Rect( 0, 0, 1, 200, 20, 1, 0, 0, 1 );

            gui.Rect( 0, 0, 1, 200, 20, 0, 1, 0, 1 );
            gui.Rect( 0, 0, 1, 200, 20, 0, 0, 1, 1 );
        }
        gui.EndLayout();

        guiShader->Bind();
        testTexture->Bind();
        gui.Update();
        testTexture->Unbind();
        guiShader->Unbind();

        projection = glm::perspective( glm::radians( fov->GetFloat() ),
            scr_width->GetFloat() / scr_height->GetFloat(), 0.1f, 10000.0f );

        window.SwapBuffers();
    }


    return 0;
}