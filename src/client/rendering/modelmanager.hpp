#include <vector>

#include "utility/vector.hpp"

#include "shadermanager.hpp"
#include "texturemanager.hpp"

#include "colour.hpp"

#pragma once

namespace modelSystem
{
	class CModel
	{
	  private:
		// Vertex Buffer
		unsigned int m_iVbo;
		// Array buffer
		unsigned int m_iVao;
		// Element buffer
		unsigned int m_iEbo;
	  public:
		struct Vertex
		{
			float x, y, z;		  // Position
			float nx, ny, nz;	  // Normal
			float u, v;			  // Texture U/V
			float cr, cg, cb;	  // Vertex CColour
			float lr, lg, lb, la; // Vertex Lighting
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

		Vector3f m_vPosition = { 0, 0, 0 };
		Vector3f m_vRotation = { 0, 0, 0 };
		Vector3f m_vSize	 = { 1, 1, 1 };

		Vector4f m_vUvOffset = { 1, 1, 0, 0 };

		CColour m_vLighting = { 1, 1, 1, 1 };

		bool m_bVisible = true;

	  protected:
		CShader *m_pShader = nullptr;
		CTexture *m_pTex   = nullptr;

		const char *m_cFilepath = nullptr;
	};

	extern std::vector<CModel *> loadedModels;

	CModel *LoadModel( const char *fp );

	void Init();
	void UnInit();

}; // namespace modelSystem

// Laziness and Convenience
using CModel = modelSystem::CModel;

void GetCubeModel( CModel &m, Vector3f size = Vector3f( 1, 1, 1 ), Vector4f uv = { 0.0f, 0.0f, 1.0f, 1.0f } );