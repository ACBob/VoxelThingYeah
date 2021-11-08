#include "shadermanager.hpp"

#include <fstream>
#include <string>

#include <cstdio>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "errorshader.hpp"

#include "filesystem.hpp"

#include "logging.hpp"

#include <regex>

std::vector<CShader *> shaderSystem::loadedShaders;

#define SHADERSYS_MAX_INCLUDE_DEPTH 2048

void ShaderPreprocess( std::string &source, int d = 0 )
{
	if ( d >= SHADERSYS_MAX_INCLUDE_DEPTH )
	{
		con_error( "Shader include depth exceeded! (%d)", SHADERSYS_MAX_INCLUDE_DEPTH );
		return;
	}

	std::regex include( "#include \"([^\"]+)\"" );

	std::smatch match;

	std::regex_search( source, match, include );

	// There aren't any includes
	if ( match.size() == 0 ) return;

	// Replace the include with the contents of the file
	// Get the file name
	std::string fileName = match[1].str();

	// for ( int i = 0; i < match.size(); i++ )
	// {
	// 	con_debug( "Match %d: %s", i, match[i].str().c_str() );
	// }

	// con_debug("Search for %s", fileName.c_str());

	bool bSuccess	= false;
	int64_t iLength = 0;
	char *pData = (char*)fileSystem::LoadFile( ("/assets/shaders/" + fileName).c_str(), iLength, bSuccess );

	if (!bSuccess)
	{
		con_error( "No such shader file: %s", fileName.c_str() );

		// Delete the include anyway
		source.erase( match[0].first, match[0].second );
	}
	else
	{
		std::string includeSource = pData;

		ShaderPreprocess( includeSource, d + 1 );

		// Replace the include with the contents of the file
		source.replace( match[0].first, match[0].second, includeSource );
	}

	// HACK: to continue in this file, we need to call ShaderPreprocess again
	if (match.size() > 1)
		ShaderPreprocess( source, d );
}

void ShaderPreprocess( char *&cSource, int d = 0 )
{
	if ( d >= SHADERSYS_MAX_INCLUDE_DEPTH )
	{
		con_error( "Shader include depth exceeded! (%d)", SHADERSYS_MAX_INCLUDE_DEPTH );
		return;
	}

	// Convert to std::string :trollface:
	std::string s( cSource );

	ShaderPreprocess( s, d );

	delete[] cSource;

	cSource = new char[s.size() + 1]();
	strcpy( cSource, s.c_str() );
}

CShader::CShader( const char *vs, const char *fs )
{
	bool bSuccess	= false;
	int64_t iLength = 0;

	char *cVertexShaderSource = (char *)fileSystem::LoadFile( vs, iLength, bSuccess );
	if ( !bSuccess )
	{
		con_error( "Cannot load Vertex %s!", fs );
		// use the error instead
		delete cVertexShaderSource;

		cVertexShaderSource = new char[strlen( ERRORVERT ) + 1];
		strcpy( cVertexShaderSource, ERRORVERT );
		cVertexShaderSource[strlen( ERRORVERT ) + 1] = '\0';
	}
	else
	{
		ShaderPreprocess(cVertexShaderSource);
	}

	unsigned int iVertexShader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( iVertexShader, 1, (const GLchar **)&cVertexShaderSource, NULL );

	char *cFragmentShaderSource = (char *)fileSystem::LoadFile( fs, iLength, bSuccess );
	if ( !bSuccess )
	{
		con_error( "Cannot load Fragment %s!", fs );
		// use the error instead
		delete cFragmentShaderSource;

		cFragmentShaderSource = new char[strlen( ERRORFRAG ) + 1];
		strcpy( cFragmentShaderSource, ERRORFRAG );
		cFragmentShaderSource[strlen( ERRORFRAG ) + 1] = '\0';
	}
	else
	{
		ShaderPreprocess(cFragmentShaderSource);
	}

	unsigned int iFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( iFragmentShader, 1, (const GLchar **)&cFragmentShaderSource, NULL );

	char *cErrBuf = new char[512];
	int iSuccess  = false;

	glCompileShader( iVertexShader );
	glGetShaderiv( iVertexShader, GL_COMPILE_STATUS, &iSuccess );
	if ( !iSuccess )
	{
		glGetShaderInfoLog( iVertexShader, 512, NULL, cErrBuf );
		con_error( "Shader compilation Error (%s):\n%s", vs, cErrBuf );
	}

	glCompileShader( iFragmentShader );
	glGetShaderiv( iFragmentShader, GL_COMPILE_STATUS, &iSuccess );
	if ( !iSuccess )
	{
		glGetShaderInfoLog( iFragmentShader, 512, NULL, cErrBuf );
		con_error( "Shader compilation Error (%s):\n%s", fs, cErrBuf );
	}

	m_iId = glCreateProgram();
	glAttachShader( m_iId, iVertexShader );
	glAttachShader( m_iId, iFragmentShader );
	glLinkProgram( m_iId );
	glGetProgramiv( m_iId, GL_LINK_STATUS, &iSuccess );
	if ( !iSuccess )
	{
		glGetProgramInfoLog( m_iId, 512, NULL, cErrBuf );
		con_error( "Shader linking Error (%s,%s):\n%s", vs, fs, cErrBuf );
	}

	glDeleteShader( iVertexShader );
	glDeleteShader( iFragmentShader );

	delete[] cVertexShaderSource;
	delete[] cFragmentShaderSource;
	delete[] cErrBuf;
}

void CShader::SetMat4( const char *name, glm::mat4 value )
{
	glUniformMatrix4fv( glGetUniformLocation( m_iId, name ), 1, false, glm::value_ptr( value ) );
}
void CShader::SetMat3( const char *name, glm::mat3 value )
{
	glUniformMatrix3fv( glGetUniformLocation( m_iId, name ), 1, false, glm::value_ptr( value ) );
}
void CShader::SetInt( const char *name, int value ) { glUniform1i( glGetUniformLocation( m_iId, name ), value ); }
void CShader::SetVec( const char *name, Vector3f value )
{
	glUniform3f( glGetUniformLocation( m_iId, name ), value.x, value.y, value.z );
}
void CShader::SetVec4( const char *name, Vector4f value )
{
	glUniform4f( glGetUniformLocation( m_iId, name ), value.x, value.y, value.z, value.w );
}
void CShader::SetUInt( const char *name, unsigned int value )
{
	glUniform1ui( glGetUniformLocation( m_iId, name ), value );
}

void CShader::Use() { glUseProgram( m_iId ); }

void shaderSystem::Init() {}

// TODO: Uniform Buffer
void shaderSystem::SetUniforms( glm::mat4 &view, glm::mat4 &projection, unsigned int ticks, int timeOfDay,
								Vector3f sunAngle )
{
	for ( CShader *s : loadedShaders )
	{
		s->Use();
		s->SetMat4( "view", view );
		s->SetMat4( "projection", projection );
		// s->SetMat4( "screen", screen );
		s->SetInt( "time", ticks );
		s->SetInt( "timeOfDay", timeOfDay );
		s->SetVec( "sunAngle", sunAngle );
	}
}

CShader *shaderSystem::LoadShader( const char *vs, const char *fs )
{
	char *vsfp = new char[strlen(vs) + 17];
	char *fsfp = new char[strlen(fs) + 17];
	strcpy(vsfp, "/assets/shaders/");
	strcpy(fsfp, "/assets/shaders/");
	strcat(vsfp, vs);
	strcat(fsfp, fs);

	CShader *shader = new CShader( vsfp, fsfp );
	loadedShaders.push_back( shader );

	delete[] vsfp;
	delete[] fsfp;

	return loadedShaders.back();
}
void shaderSystem::UnInit()
{
	// Unload shaders
	for ( CShader *s : loadedShaders )
		delete s;
}
