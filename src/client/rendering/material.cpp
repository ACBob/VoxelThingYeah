#include "material.hpp"

#include <glad/glad.h>

#include "logging.hpp"
#include "filesystem.hpp"

#include "lodepng.h"

#include <glm/gtc/type_ptr.hpp>

#include <regex>


namespace materialSystem {

    std::map<std::string, CTexture*> vTextures;
    std::vector<CShader*> vShaders;

    bool Init() {
        return true;
    }
    void Uninit() {
        for (auto& shader : vShaders) {
            delete shader;
        }
        vShaders.clear();
        for (auto& texture : vTextures) {
            delete texture.second;
        }
        vTextures.clear();
    }

    CTexture *LoadTexture(const std::string& path) {
        std::string filePath = "/assets/textures/" + path;

        if (vTextures.find(filePath) != vTextures.end()) {
            return vTextures[filePath];
        }

        CTexture *texture = new CTexture( filePath );
        vTextures[filePath] = texture;
        return texture;
    }

    CShader *LoadShader(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string vertexFilePath = std::string("/assets/shaders/") + vertexPath;
        std::string fragmentFilePath = std::string("/assets/shaders/") + fragmentPath;
        CShader *shader = new CShader( vertexFilePath, fragmentFilePath );
        vShaders.push_back(shader);
        return shader;
    }

    CTexture::CTexture( const std::string &filepath )
    {
        m_sFilepath = filepath;

        m_uchImageData = {
            255, 0, 255, 255,
            0, 0, 0, 255,
            255, 0, 255, 255,
            0, 0, 0, 255,
        };

        m_nHeight = 2;
        m_nWidth = 2;

        glGenTextures( 1, &m_nTextureID );
        glBindTexture( GL_TEXTURE_2D, m_nTextureID );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_nWidth, m_nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &m_uchImageData[0] );
        glBindTexture( GL_TEXTURE_2D, 0 );

        Load();
    }

    void CTexture::Load()
    {
        bool bSuccess		= false;
        int64_t iFileLength = 0;

        m_uchImageData.clear();

        const unsigned char *cPNGData = fileSystem::LoadFile( m_sFilepath.c_str(), iFileLength, bSuccess );
        uint iError				= lodepng::decode( m_uchImageData, m_nWidth, m_nHeight, cPNGData, iFileLength );
        delete[] cPNGData;
        if ( iFileLength < 10 || !bSuccess || iError != 0 )
        {
            if ( iError != 0 )
            {
                con_error( "LodePNG Error: %s", lodepng_error_text( iError ) );
            }

            con_error( "Invalid PNG %s, loading error texture...", m_sFilepath.c_str() );

            // The image data is RGBA
            // So the data can be written like this for a classic source-style checkerboard
            m_uchImageData = {
                255, 0, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 0, 255, 255,
            };

            m_nWidth  = 2;
            m_nHeight = 2;
        }

        glBindTexture( GL_TEXTURE_2D, m_nTextureID );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_nWidth, m_nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &m_uchImageData[0] );
    }


    CTexture::~CTexture()
    {
        glDeleteTextures( 1, &m_nTextureID );
    }

    void CTexture::Bind( int slot )
    {
        glActiveTexture( GL_TEXTURE0 + slot );
        glBindTexture( GL_TEXTURE_2D, m_nTextureID );
    }
    void CTexture::Unbind(int slot)
    {
        glActiveTexture( GL_TEXTURE0 + slot );
        glBindTexture( GL_TEXTURE_2D, 0 );
    }

    void ShaderPreprocessor(std::string& shader, int depth) {
        if ( depth >= MATSYS_MAX_SHADER_INCLUDE_DEPTH) {
            con_error("Shader include depth exceeded");
            return;
        }

        std::regex include( "#include \"([^\"]+)\"" );

        std::smatch match;

        std::regex_search( shader, match, include );

        // There aren't any includes
        if ( match.size() == 0 )
            return;

        std::string includeFile = "/assets/shaders/" + match[1].str();

        bool bSuccess = false;
        int64_t iFileLength = 0;
        const char *sIncludeFile = (char*)fileSystem::LoadFile( includeFile.c_str(), iFileLength, bSuccess );

        if ( !bSuccess )
        {
            con_error( "Failed to load shader include %s", includeFile.c_str() );

            // delete the include
            shader.erase( match[0].first, match[0].second );
        }
        else
        {
            std::string includeContents( sIncludeFile, iFileLength );

            ShaderPreprocessor( includeContents, depth + 1 );

            shader.replace( match[0].first, match[0].second, includeContents );

            delete sIncludeFile;
        }

        // HACK: continue searching for includes
        if ( match.size() > 0 )
            ShaderPreprocessor( shader, depth );
    }

    CShader::CShader( const std::string &vertexShaderFilepath, const std::string &fragmentShaderFilepath )
    {
        m_sVertexShaderFilepath = vertexShaderFilepath;
        m_sFragmentShaderFilepath = fragmentShaderFilepath;

        unsigned int nVertexShader = glCreateShader( GL_VERTEX_SHADER );
        unsigned int nFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

        bool bSuccess = false;
        int64_t iFileLength = 0;
        std::string sVertexShaderSource = (char *)fileSystem::LoadFile( m_sVertexShaderFilepath.c_str(), iFileLength, bSuccess );

        if ( !bSuccess )
        {
            con_error( "Failed to load vertex shader %s", m_sVertexShaderFilepath.c_str() );
        }

        ShaderPreprocessor( sVertexShaderSource, 0 );

        const char *sVertexShaderSourceCStr = sVertexShaderSource.c_str();
        glShaderSource( nVertexShader, 1, &sVertexShaderSourceCStr, NULL );
        
        bSuccess = false;
        iFileLength = 0;
        std::string sFragmentShaderSource = (char *)fileSystem::LoadFile( m_sFragmentShaderFilepath.c_str(), iFileLength, bSuccess );

        if ( !bSuccess )
        {
            con_error( "Failed to load fragment shader %s", m_sFragmentShaderFilepath.c_str() );
        }

        ShaderPreprocessor( sFragmentShaderSource, 0 );

        const char *sFragmentShaderSourceCStr = sFragmentShaderSource.c_str();
        glShaderSource( nFragmentShader, 1, &sFragmentShaderSourceCStr, NULL );

        glCompileShader( nVertexShader );
        glCompileShader( nFragmentShader );

        int iSuccess = 0;
        char sInfoLog[1024];
        glGetShaderiv( nVertexShader, GL_COMPILE_STATUS, &iSuccess );
        if ( !iSuccess )
        {
            glGetShaderInfoLog( nVertexShader, 1024, NULL, sInfoLog );
            con_error( "Failed to compile vertex shader %s: %s", m_sVertexShaderFilepath.c_str(), sInfoLog );
        }

        glGetShaderiv( nFragmentShader, GL_COMPILE_STATUS, &iSuccess );
        if ( !iSuccess )
        {
            glGetShaderInfoLog( nFragmentShader, 1024, NULL, sInfoLog );
            con_error( "Failed to compile fragment shader %s: %s", m_sFragmentShaderFilepath.c_str(), sInfoLog );
        }

        m_nShaderID = glCreateProgram();
        glAttachShader( m_nShaderID, nVertexShader );
        glAttachShader( m_nShaderID, nFragmentShader );
        glLinkProgram( m_nShaderID );

        glGetProgramiv( m_nShaderID, GL_LINK_STATUS, &iSuccess );
        if ( !iSuccess )
        {
            glGetProgramInfoLog( m_nShaderID, 1024, NULL, sInfoLog );
            con_error( "Failed to link shader program: %s", sInfoLog );
        }

        glDeleteShader( nVertexShader );
        glDeleteShader( nFragmentShader );
    }

    CShader::~CShader()
    {
        glDeleteProgram( m_nShaderID );
    }

    void CShader::Bind()
    {
        glUseProgram( m_nShaderID );
    }
    void CShader::Unbind()
    {
        glUseProgram( 0 );
    }

    void CShader::SetUInt( const char *name, unsigned int value )
    {
        glUniform1ui( glGetUniformLocation( m_nShaderID, name ), value );
    }
    void CShader::SetFloat( const char *name, float value )
    {
        glUniform1f( glGetUniformLocation( m_nShaderID, name ), value );
    }
    void CShader::SetVec3( const char *name, float x, float y, float z )
    {
        glUniform3f( glGetUniformLocation( m_nShaderID, name ), x, y, z );
    }
    void CShader::SetMat4( const char *name, glm::mat4 value )
    {
        glUniformMatrix4fv( glGetUniformLocation( m_nShaderID, name ), 1, GL_FALSE, glm::value_ptr( value ) );
    }

}