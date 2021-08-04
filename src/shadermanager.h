// TODO: shader manager

#include <glm/glm.hpp>

#pragma once

class Shader
{
	public:
	// Opengl Id
	// TODO: abstract
	unsigned int id;

	Shader(const char* vs, const char* fs);

	// Uniforms
	void SetMat4(const char* name, glm::mat4 value);

	void Use();
};