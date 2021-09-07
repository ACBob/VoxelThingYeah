#include <vector>

#include "utility/vector.hpp"

#include "shadermanager.hpp"
#include "texturemanager.hpp"

#include "modelrenderer.hpp"

#pragma once

namespace modelSystem
{
	class CModel
	{
	  public:
		struct sVertex
		{
			float x, y, z;	  // Position
			float nx, ny, nz; // Normal
			float u, v;		  // Texture U/V
							  // TODO: Texture, Lighting, Etc.
		};

		struct sFace
		{
			Face( int a = 0, int b = 0, int c = 0 )
			{
				v	= a;
				vv	= b;
				vvv = c;
			};
			unsigned int v, vv, vvv;
		};

		CModel( std::vector<sVertex> verts = {}, std::vector<sFace> faces = {} );
		~CModel();

		void Update();
		void Render();

		void SetShader( CShader *pShader );
		CShader *GetShader();

		void SetTexture( CTexture *pTexture );
		CTexture *GetTexture();

		std::vector<Vertex> m_vertices;
		std::vector<Face> m_faces;

		CVector m_vPosition = { 0, 0, 0 };
		CVector m_vRotation = { 0, 0, 0 };
		CVector m_vSize		= { 1, 1, 1 };

	  private:
		CShader *pShader = nullptr;
		CTexture *pTex   = nullptr;

		CModelRenderer *pRenderer = nullptr;

		const char *cFilepath = nullptr;
	};

	extern std::vector<CModel *> loadedModels;

	CModel *LoadModel( const char *fp );

	void Init();
	void UnInit();

}; // namespace modelSystem

// Laziness and Convenience
using CModel = modelSystem::CModel;

void GetCubeModel( CModel &m, CVector size = CVector( 1, 1, 1 ) );