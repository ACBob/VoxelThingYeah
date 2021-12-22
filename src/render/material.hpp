// -- Material System
// Handles shaders & textures at the same time.

#include <vector>
#include <map>
#include <string>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#pragma once

// How far you can include in a shader.
#define MATSYS_MAX_SHADER_INCLUDE_DEPTH 128

namespace rendering {
    namespace materials {

        class CTexture
        {
            public:
                CTexture( const std::string &filepath );
                ~CTexture();

                void Load();
                
                std::vector<unsigned char> m_uchImageData;

                unsigned int m_nHeight;
                unsigned int m_nWidth;

                void Bind( int slot = 0 );
                void Unbind( int slot = 0 );

            protected:
                unsigned int m_nTextureID;
                std::string m_sFilepath;
        };


        class CShader
        {
            public:
                CShader( const std::string &vertexShaderFilepath, const std::string &fragmentShaderFilepath );
                ~CShader();

                void SetUInt( const char *name, unsigned int value );
                void SetFloat( const char *name, float value );
                void SetVec3( const char *name, float x, float y, float z );
                void SetMat4( const char *name, glm::mat4 mat );

                void Bind();
                void Unbind();

            protected:
                unsigned int m_nShaderID;
                std::string m_sVertexShaderFilepath;
                std::string m_sFragmentShaderFilepath;
        };

        bool Init();
        void Uninit();

        void UpdateUniforms( glm::mat4 projection, glm::mat4 view, glm::mat4 screen );

        CTexture* LoadTexture( const std::string &filepath );
        CShader* LoadShader( const std::string &vertexShaderFilepath, const std::string &fragmentShaderFilepath );

        CShader* GetNamedShader( const std::string &name );
    } // namespace materials
}