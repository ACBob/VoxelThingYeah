#include "render.hpp"

// We accept the responsibility of handling ALL of the rendering code
// The end result being nobody but us handles raw gl calls

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "shared/logging.hpp"

bool rendering::LoadGL() {

    SDL_GL_LoadLibrary(NULL);

    SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );

    SDL_LogSetPriority( SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO );

    return true;
}

bool rendering::Init() {

    if ( !gladLoadGLLoader( SDL_GL_GetProcAddress ) )
    {
        con_error("Failed to initialize GLAD!");
        return false;
    }

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_BLEND );
    glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
    glBlendEquation( GL_FUNC_ADD );

    if (!materials::Init())
        return false;

    return true;
}

void rendering::Uninit() {
    materials::Uninit();
}