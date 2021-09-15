// TODO: shader manager

#include <glm/glm.hpp>

#include "utility/vector.hpp"

#include <vector>

#pragma once

namespace shaderSystem
{
	class CShader
	{
	  public:
		// Opengl Id
		// TODO: abstract
		unsigned int m_iId;

		CShader( const char *vs, const char *fs );

		// Uniforms
		void SetMat4( const char *name, glm::mat4 value );
		void SetMat3( const char *name, glm::mat3 value );
		void SetVec( const char *name, CVector value );
		void SetInt( const char *name, int value );
		void SetUInt( const char *name, unsigned int value );

		void Use();
	};

	void Init();
	void UnInit();

	void SetUniforms( glm::mat4 &viewScreen, glm::mat4 &viewModel, glm::mat4 &projection, glm::mat4 &screen, unsigned int ticks, int timeOfDay,
					  CVector sunAngle );

	CShader *LoadShader( const char *vs, const char *fs );

	extern std::vector<CShader *> loadedShaders;

	// unsigned int uniformBuffer;
}; // namespace shaderSystem

using CShader = shaderSystem::CShader;