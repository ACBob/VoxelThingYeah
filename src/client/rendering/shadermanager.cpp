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

	PHYSFS_File *f = PHYSFS_openRead( vs );
	int64_t fl	   = PHYSFS_fileLength( f );
	char vsc[fl + 1];

	int64_t rl = PHYSFS_readBytes( f, &vsc, fl );

	vsc[fl] = '\0';

	if ( rl != fl )
	{
		printf( "PHYSFS Error!\n%s\n", PHYSFS_getLastError() );
	}

	const char *c		= &vsc[0];
	unsigned int vertSh = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertSh, 1, (const GLchar **)&c, NULL );

	PHYSFS_close( f );

	f  = PHYSFS_openRead( fs );
	fl = PHYSFS_fileLength( f );
	char fsc[fl + 1];

	rl = PHYSFS_readBytes( f, &fsc, fl );

	fsc[fl] = '\0';

	if ( rl != fl )
	{
		printf( "PHYSFS Error!\n%s\n", PHYSFS_getLastError() );
	}

	const char *c1		= &fsc[0];
	unsigned int fragSh = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragSh, 1, (const GLchar **)&c1, NULL );

	PHYSFS_close( f );

	char err[512];
	int success;

	// printf("%s\n",c);

	// printf("%s\n",c1);

	glCompileShader( vertSh );
	glGetShaderiv( vertSh, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( vertSh, 512, NULL, err );
		printf( "Shader compilation Error (%s):\n%s", vs, err );
	}

	glCompileShader( fragSh );
	glGetShaderiv( fragSh, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( fragSh, 512, NULL, err );
		printf( "Shader compilation Error (%s):\n%s", fs, err );
	}

	m_iId = glCreateProgram();
	glAttachShader( m_iId, vertSh );
	glAttachShader( m_iId, fragSh );
	glLinkProgram( m_iId );
	glGetProgramiv( m_iId, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( m_iId, 512, NULL, err );
		printf( "Shader linking Error (%s,%s):\n%s", vs, fs, err );
	}

	glDeleteShader( vertSh );
	glDeleteShader( fragSh );
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
