// TODO: shader manager

#include <glm/glm.hpp>

#include <vector>

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

class ShaderManager
{
	public:
		ShaderManager();
		~ShaderManager();

		void SetUniforms(
			glm::mat4 &view,
			glm::mat4 &projection
		);

		Shader* LoadShader(const char* vs, const char* fs);

		std::vector<Shader*> loadedShaders;
		
		unsigned int uniformBuffer;
};