// TODO: shader manager

#include <glm/glm.hpp>

#include "utility/vector.hpp"

#include <vector>

#pragma once

namespace shaderSystem
{
	class Shader
	{
	  public:
		// Opengl Id
		// TODO: abstract
		unsigned int id;

		Shader( const char *vs, const char *fs );

		// Uniforms
		void SetMat4( const char *name, glm::mat4 value );
		void SetMat3( const char *name, glm::mat3 value );
		void SetVec( const char *name, Vector value );
		void SetInt( const char *name, int value );
		void SetUInt( const char *name, unsigned int value );

		void Use();
	};

	void Init();
	void UnInit();

	void SetUniforms( glm::mat4 &view, glm::mat4 &projection, glm::mat4 &screen, unsigned int ticks, int timeOfDay,
					  Vector sunAngle );

	Shader *LoadShader( const char *vs, const char *fs );

	extern std::vector<Shader *> loadedShaders;

	// unsigned int uniformBuffer;
}; // namespace shaderSystem

using Shader = shaderSystem::Shader;