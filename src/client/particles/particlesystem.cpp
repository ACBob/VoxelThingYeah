#include "particlesystem.hpp"

const CModel::Vertex particlePlane[4] = {
	// POSITION              NORMAL                UV
	{ -0.5f, -0.5f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f },
	{  0.5f, -0.5f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f },
	{  0.5f,  0.5f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f },
	{ -0.5f,  0.5f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f }
};
const CModel::Face particlePlaneFaces[2] = {
	{ 2, 1, 0 },
	{ 0, 3, 2 }
};

namespace particleSystem
{
	void Init()
	{
		particleMdl = new CModel();
		std::copy( particlePlane, particlePlane + 4, std::back_inserter( particleMdl->m_vertices ) );
		std::copy( particlePlaneFaces, particlePlaneFaces + 2, std::back_inserter( particleMdl->m_faces ) );
		particleMdl->Update();

		particleMdl->SetShader(shaderSystem::LoadShader("shaders/particle.vert", "shaders/particle.frag"));
		particleMdl->SetTexture(materialSystem::LoadTexture("terrain.png"));

	}
	void UnInit()
	{
		delete particleMdl;
	}

	std::vector<CParticle> particles;
	std::vector<CParticleEmitter> particleEmitters;

	CModel *particleMdl = nullptr;
} // namespace particleSystem