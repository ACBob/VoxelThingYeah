#include "particlesystem.hpp"

const CModel::Vertex particlePlane[4] = {
	// POSITION              NORMAL                UV
	{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f },
	{ 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
	{ -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f } };
const CModel::Face particlePlaneFaces[2] = { { 0, 1, 2 }, { 2, 3, 0 } };

namespace particleSystem
{
	void Init()
	{
		particleMdl = new CModel();
		std::copy( particlePlane, particlePlane + 4, std::back_inserter( particleMdl->m_vertices ) );
		std::copy( particlePlaneFaces, particlePlaneFaces + 2, std::back_inserter( particleMdl->m_faces ) );
		particleMdl->Update();

		particleMdl->SetShader( shaderSystem::LoadShader( "particle.vert", "particle.frag" ) );
		particleMdl->SetTexture( materialSystem::LoadTexture( "test.png" ) );
	}
	void UnInit() { delete particleMdl; }

	CModel *particleMdl = nullptr;
} // namespace particleSystem