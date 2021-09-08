#include "shadermanager.hpp"

#include <fstream>
#include <string>

#include <cstdio>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "physfs.h"

std::vector<CShader *> shaderSystem::loadedShaders;

CShader::CShader( const char *vs, const char *fs )
{
	// TODO: Replace with FileSystem
	// I tried for a bit in vain but it got in the way of fun bobj stuff...

	char *cVertexShaderSource = (char *)fileSystem::LoadFile(vs);

	unsigned int iVertexShader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( iVertexShader, 1, (const GLchar **)&cVertexShaderSource, NULL );

	char *cFragmentShaderSource = (char *)fileSystem::LoadFile(fs);

	unsigned int iFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( iFragmentShader, 1, (const GLchar **)&cFragmenthaderSource, NULL );

	char err[512];
	int success;

	// printf("%s\n",c);

	// printf("%s\n",c1);

	glCompileShader( iVertexShader );
	glGetShaderiv( iVertexShader, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( iVertexShader, 512, NULL, err );
		printf( "Shader compilation Error (%s):\n%s", vs, err );
	}

	glCompileShader( iFragmentShader );
	glGetShaderiv( iFragmentShader, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( iFragmentShader, 512, NULL, err );
		printf( "Shader compilation Error (%s):\n%s", fs, err );
	}

	m_iId = glCreateProgram();
	glAttachShader( m_iId, iVertexShader );
	glAttachShader( m_iId, iFragmentShader );
	glLinkProgram( m_iId );
	glGetProgramiv( m_iId, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( m_iId, 512, NULL, err );
		printf( "Shader linking Error (%s,%s):\n%s", vs, fs, err );
	}

	glDeleteShader( iVertexShader );
	glDeleteShader( iFragmentShader );
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
void CShader::SetVec( const char *name, CVector value )
{
	glUniform3f( glGetUniformLocation( m_iId, name ), value.x, value.y, value.z );
}
void CShader::SetUInt( const char *name, unsigned int value )
{
	glUniform1ui( glGetUniformLocation( m_iId, name ), value );
}

void CShader::Use() { glUseProgram( m_iId ); }

void shaderSystem::Init() {}

// TODO: Uniform Buffer
void shaderSystem::SetUniforms( glm::mat4 &view, glm::mat4 &projection, glm::mat4 &screen, unsigned int ticks,
								int timeOfDay, CVector sunAngle )
{
	for ( CShader *s : loadedShaders )
	{
		s->Use();
		s->SetMat4( "view", view );
		s->SetMat4( "projection", projection );
		s->SetMat4( "screen", screen );
		s->SetInt( "time", ticks );
		s->SetInt( "timeOfDay", timeOfDay );
		s->SetVec( "sunAngle", sunAngle );
	}
}

CShader *shaderSystem::LoadShader( const char *vs, const char *fs )
{
	CShader *shader = new CShader( vs, fs );
	loadedShaders.push_back( shader );

	return loadedShaders.back();
}
void shaderSystem::UnInit()
{
	// Unload shaders
	for ( CShader *s : loadedShaders )
		delete s;
}
