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

		void SetShader( CShader *shader );
		CShader *GetShader();

		void SetTexture( CTexture *texture );
		CTexture *GetTexture();

		std::vector<Vertex> vertices;
		std::vector<Face> faces;

		CVector position = { 0, 0, 0 };
		CVector rotation = { 0, 0, 0 };
		CVector size		= { 1, 1, 1 };

	  private:
		CShader *shader = nullptr;
		CTexture *tex   = nullptr;

		CModelRenderer *renderer = nullptr;

		const char *fp = nullptr;
	};

	extern std::vector<CModel *> loadedModels;

	CModel *LoadModel( const char *fp );

	void Init();
	void UnInit();

}; // namespace modelSystem

// Laziness and Convenience
using CModel = modelSystem::CModel;

void GetCubeModel( CModel &m, CVector size = CVector( 1, 1, 1 ) );