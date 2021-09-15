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
		struct Vertex
		{
			float x, y, z;	  // Position
			float nx, ny, nz; // Normal
			float u, v;		  // Texture U/V
							  // TODO: Texture, Lighting, Etc.
		};

		struct Face
		{
			Face( int a = 0, int b = 0, int c = 0 )
			{
				v	= a;
				vv	= b;
				vvv = c;
			};
			unsigned int v, vv, vvv;
		};

		CModel( std::vector<Vertex> verts = {}, std::vector<Face> faces = {} );
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

		CVector m_vUvOffset = { 0, 0 };

		bool m_bVisible = true;

	  private:
		CShader *m_pShader = nullptr;
		CTexture *m_pTex   = nullptr;

		CModelRenderer *m_pRenderer = nullptr;

		const char *m_cFilepath = nullptr;
	};

	extern std::vector<CModel *> loadedModels;

	CModel *LoadModel( const char *fp );

	void Init();
	void UnInit();

}; // namespace modelSystem

// Laziness and Convenience
using CModel = modelSystem::CModel;

void GetCubeModel( CModel &m, CVector size = CVector( 1, 1, 1 ), CVector uv = CVector( 0.0f, 0.0f, 1.0f, 1.0f ) );