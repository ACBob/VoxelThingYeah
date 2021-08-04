#include "shadermanager.h"

#include <fstream>
#include <string>

#include <cstdio>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vs, const char* fs)
{
	// TODO: physfs filesystem
	std::ifstream vsf(vs);
	// not the free software foundation
	std::ifstream fsf(fs);

	std::string vsc(
		(std::istreambuf_iterator<char>(vsf)),
		(std::istreambuf_iterator<char>())
	);
	std::string fsc(
		(std::istreambuf_iterator<char>(fsf)),
		(std::istreambuf_iterator<char>())
	);

	fsf.close();
	vsf.close();

	char err[512];
	int success;

	unsigned int vertSh = glCreateShader(GL_VERTEX_SHADER);
	const char* c = vsc.c_str();
	glShaderSource(vertSh, 1, &c, NULL);

	// printf("%s\n",c);

	unsigned int fragSh = glCreateShader(GL_FRAGMENT_SHADER);
	const char* c1 = fsc.c_str();
	glShaderSource(fragSh, 1, &c1, NULL);
	
	// printf("%s\n",c1);

	glCompileShader(vertSh);
	glGetShaderiv(vertSh, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertSh, 512, NULL, err);
		printf("Shader compilation Error (%s):\n%s", vs, err);
	}

	glCompileShader(fragSh);
	glGetShaderiv(fragSh, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragSh, 512, NULL, err);
		printf("Shader compilation Error (%s):\n%s", fs, err);
	}

	id = glCreateProgram();
	glAttachShader(id, vertSh);
	glAttachShader(id, fragSh);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 512, NULL, err);
		printf("Shader linking Error (%s,%s):\n%s", vs,fs, err);
	}

	glDeleteShader(vertSh);
	glDeleteShader(fragSh);
}

void Shader::SetMat4(const char* name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, false, glm::value_ptr(value));
}

void Shader::Use()
{
	glUseProgram(id);
}