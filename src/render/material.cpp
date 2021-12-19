#include "material.hpp"

#include <glad/glad.h>

#include "logging.hpp"
#include "filesystem.hpp"

#include "lodepng.h"
#include "utility/toml.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <regex>


namespace materialSystem {

    std::map<std::string, CTexture*> vTextures;
    std::vector<CShader*> vShaders;
    std::map<std::string, CShader*> vShaderMap;

    bool Init() {
        // Attempt to load the assets/shaders/shaders.toml file
        bool bSucceed = false;
        int64_t nFileSize = 0;
        char* pFileData = (char *)fileSystem::LoadFile("assets/shaders/shaders.toml", nFileSize, bSucceed);

        // Don't error out if the file doesn't exist
        if (!bSucceed && fileSystem::Exists("assets/shaders/shaders.toml")) {
            con_error("Failed to load assets/shaders/shaders.toml");
            return false;
        }

        if (!fileSystem::Exists("assets/shaders/shaders.toml")) {
            con_error("assets/shaders/shaders.toml does not exist");
            pFileData = (char *)malloc(1);
            pFileData[0] = '\0';
        }

        // Parse the TOML file
        toml::table root = toml::parse(pFileData);

        for ( auto &k : root ) {
            
            // the name is the key
            std::string sName = k.first;

            toml::table *pShaderDef = k.second.as_table();

            std::string vertexShader;
            std::string fragmentShader;

            vertexShader = pShaderDef->get("vertex")->value_or<std::string>("");
            if (vertexShader.empty()) {
                con_error("Vertex shader for %s is empty", sName.c_str());
                continue;
            }

            fragmentShader = pShaderDef->get("fragment")->value_or<std::string>("");
            if (fragmentShader.empty()) {
                con_error("Fragment shader for %s is empty", sName.c_str());
                continue;
            }

            // Create the shader
            CShader* pShader = LoadShader(vertexShader, fragmentShader);
            if (!pShader) {
                con_error("Failed to load shader %s", sName.c_str());
                continue;
            }

            // map the name to the shader
            vShaderMap[sName] = pShader;
        }

        return true;
    }
    void Uninit() {
        for (auto& shader : vShaders) {
            delete shader;
        }
        vShaders.clear();
        vShaderMap.clear();
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

    CShader *GetNamedShader(const std::string& name) {
        if (vShaderMap.find(name) != vShaderMap.end()) {
            return vShaderMap[name];
        }
        con_error("Could not find shader %s", name.c_str());
        return nullptr; // TODO: error shader
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
        const char *cVertexShaderSource = (char *)fileSystem::LoadFile( m_sVertexShaderFilepath.c_str(), iFileLength, bSuccess );
        std::string sVertexShaderSource;

        if ( !bSuccess )
        {
            con_error( "Failed to load vertex shader %s", m_sVertexShaderFilepath.c_str() );
        }
        else
        {
            sVertexShaderSource = std::string( cVertexShaderSource, iFileLength );
        }

        ShaderPreprocessor( sVertexShaderSource, 0 );

        const char *sVertexShaderSourceCStr = sVertexShaderSource.c_str();
        glShaderSource( nVertexShader, 1, &sVertexShaderSourceCStr, NULL );
        
        bSuccess = false;
        iFileLength = 0;
        const char *FragmentShaderSource = (char *)fileSystem::LoadFile( m_sFragmentShaderFilepath.c_str(), iFileLength, bSuccess );
        std::string sFragmentShaderSource;

        if ( !bSuccess )
        {
            con_error( "Failed to load fragment shader %s", m_sFragmentShaderFilepath.c_str() );
        }
        else
        {
            sFragmentShaderSource = std::string( FragmentShaderSource, iFileLength );
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


    void UpdateUniforms( glm::mat4 projection, glm::mat4 view, glm::mat4 screen )
    {
        for ( CShader *shd : vShaders )
        {
            shd->Bind();
            shd->SetMat4( "projection", projection );
            shd->SetMat4( "view", view );
            shd->SetMat4( "screen", screen );
            shd->Unbind();
        }
    }

}