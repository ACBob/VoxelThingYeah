#include "logging.hpp"

#include "sdlstuff/sdlwindow.hpp"

#ifdef _WIN32
	#include <stdio.h>
#endif

#include "cvar_clientside.hpp"

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "shared/filesystem.hpp"
#include "network/cl_network.hpp"

#include "render/render.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#include "gui/shtoigui.hpp"

#include "world/world.hpp"
#include "world/chunk.hpp"

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

    con_info("Create Network Client");
    CClient client;

    // Try and connect to the server
    if (!client.Connect("127.0.0.1", DEFAULT_PORT))
        window.Panic("Failed to connect to server!");


    CInputManager inputManager;
    window.m_pInputMan = &inputManager;

    con_info("Init renderer");
    rendering::LoadGL();
    window.GatherCTX();
    if (!rendering::Init())
        window.Panic("Failed to initialize rendering!");
    atexit(rendering::Uninit);

    con_info("Ok cool done");

    window.SetIcon( "logo64.png" );

    glm::mat4 projection = glm::perspective( glm::radians( fov->GetFloat() ),
        scr_width->GetFloat() / scr_height->GetFloat(), 0.1f, 10000.0f );
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 screen = glm::ortho( 0.0f, scr_width->GetFloat(), scr_height->GetFloat(), 0.0f, -1.0f, 1.0f );

    Vector3f camPos = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f camLook = Vector3f(0.0f, 0.0f, -1.0f);
    Vector3f camUp = Vector3f(0.0f, 1.0f, 0.0f);

    rendering::materials::CShader *guiShader = rendering::materials::GetNamedShader( "gui" );
    rendering::materials::CTexture *testTexture = rendering::materials::LoadTexture( "filledSquare.png" );
    
    float camPitch = 0.0f;
    float camYaw = 0.0f;

    CShtoiGUI gui( ShtoiGUI_displayMode::TwoDee, window.GetSize().x, window.GetSize().y );

    CClientWorld testingWorld;
    testingWorld.setChunkSize(16, 16, 16);
    CClientChunk *c = testingWorld.createChunk(0, 0, 0);
    for (int i = 0; i < c->getSizeX() * c->getSizeY() * c->getSizeZ(); i ++)
        c->set(i, rand() & 2 );
    c->constructModel();

    CPlayerEntity *player = testingWorld.createEntity<CPlayerEntity>();
    player->setposition(Vector3f(8.0f, 24.0f, 8.0f));
    player->setvisible(false);
    player->setgravity(false);
    player->setmaxSpeed(4.3f);

    client.m_pEntity = player;

    CCameraEntity *playerCam = testingWorld.createEntity<CCameraEntity>();
    playerCam->setparent(player);
    playerCam->setparentPositionOffset(Vector3f(0.0f, 1.62f, 0.0f));

    // inputManager.m_bInGui = true;

    float deltaTime = 0.0f;
    
    uint32_t lastTime = 0;
    uint32_t curTime = 0;

    char* guiBuf = new char[256];

    while ( !window.m_bShouldClose )
    {
        window.PollEvents();
        inputManager.Update();

        client.Update();

        glClearColor( 0.0f, 0.5f, 0.5f, 1.0f );
        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

        // camera rotation
        camPitch -= inputManager.m_vMouseMovement.y * 0.1f;
        camYaw -= inputManager.m_vMouseMovement.x * 0.1f;

        testingWorld.update(deltaTime);

        camPos = playerCam->getposition();
        camPitch = fminf( fmaxf( camPitch, -89.9f ), 89.9f );

        while (camYaw > 360.0f)
            camYaw -= 360.0f;
        while (camYaw < 0.0f)
            camYaw += 360.0f;

        player->m_inForward = inputManager.m_bInputState[CONTROLS_FRONT];
        player->m_inBackward = inputManager.m_bInputState[CONTROLS_BACK];
        player->m_inLeft = inputManager.m_bInputState[CONTROLS_LEFT];
        player->m_inRight = inputManager.m_bInputState[CONTROLS_RIGHT];
        player->m_inUp = inputManager.m_bInputState[CONTROLS_UP];
        player->m_inDown = inputManager.m_bInputState[CONTROLS_DOWN];

        camLook = Vector3f(0, 0, -1.0f).Rotate( Vector3f(0, 1, 0), camPitch * DEG2RAD ).Rotate( Vector3f(1, 0, 0), camYaw * DEG2RAD );

        playerCam->setrotation(camLook);
        player->setlookRotation( Vector3f( camPitch, camYaw ) );

        view = glm::lookAt(glm::vec3(camPos.x, camPos.y, camPos.z), glm::vec3(camPos.x + camLook.x, camPos.y + camLook.y, camPos.z + camLook.z), glm::vec3(camUp.x, camUp.y, camUp.z));

        rendering::materials::UpdateUniforms( projection, view, screen );

        testingWorld.render();
        
        // gui
        snprintf(guiBuf, 256, "<%.3f, %.3f, %.3f>", player->getposition().x, player->getposition().y, player->getposition().z);
        gui.Label(guiBuf, 0, 16, 0, 16);

        snprintf(guiBuf, 256, "<%.3f, %.3f, %.3f>", player->getvelocity().x, player->getvelocity().y, player->getvelocity().z);
        gui.Label(guiBuf, 0, 32, 0, 16);

        snprintf(guiBuf, 256, "%f (%f)", deltaTime, 1.0f / deltaTime);
        gui.Label(guiBuf, 0, 48, 0, 16);

        snprintf(guiBuf, 256, "<%.3f, %.3f, %.3f>", player->getForward().x, player->getForward().y, player->getForward().z);
        gui.Label(guiBuf, 0, 64, 0, 16);

        gui.Update( inputManager.m_vMousePos.x, inputManager.m_vMousePos.y, inputManager.m_iMouseState );

        if ( inputManager.m_bKeyboardState[KBD_BACKSPACE] && !inputManager.m_bOldKeyboardState[KBD_BACKSPACE] )
        {
            con_info("Dumping testing world as OBJ");
            std::string obj = testingWorld.getChunk(0, 0, 0)->m_model.DumpOBJ();
            std::string filename = "test.obj";

            con_info("Writing to %s", filename.c_str());
            bool bSuccess = false;
            fileSystem::WriteFile(filename.c_str(), (unsigned char*)obj.c_str(), obj.length(), bSuccess);
            if (bSuccess)
                con_info("Success!");
            else
                con_error("Failed!");
        }

        window.SwapBuffers();

        curTime = window.GetMS();
        deltaTime = (curTime - lastTime) / 1000.0f;
        lastTime = curTime;
    }


    return 0;
}