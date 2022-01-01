#include "model.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "material.hpp"

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "logging.hpp"
#include "filesystem.hpp"

#include "utility/toml.hpp"

#include <cstring>

using namespace rendering;
using namespace rendering::materials;

std::vector<rendering::models::CModel*> g_models;
std::map<std::string, rendering::models::CModel*> g_modelMap;

namespace rendering::models {

	bool Init() {
		// Attempt to load the assets/models/models.toml file
        bool bSucceed = false;
        int64_t nFileSize = 0;
        char* pFileData = (char *)fileSystem::LoadFile("assets/models/models.toml", nFileSize, bSucceed);

        // Don't error out if the file doesn't exist
        if (!bSucceed && fileSystem::Exists("assets/models/models.toml")) {
            con_error("Failed to load assets/models/models.toml");
            return false;
        }

        if (!fileSystem::Exists("assets/models/models.toml")) {
            con_error("assets/models/models.toml does not exist");
            pFileData = (char *)malloc(1);
            pFileData[0] = '\0';
        }

        // Parse the TOML file
        toml::table root = toml::parse(pFileData);

		// Load
		toml::table *modelTable = root.get("models")->as_table();
		for (auto &k : *modelTable) {
			std::string modelName = k.first;
			std::string modelFilepath = k.second.value_or("");

			if (modelFilepath.empty()) {
				con_error("Model %s has no filepath", modelName.c_str());
				continue;
			}

			// Create the model
			CModel* pModel = new CModel();
			pModel->LoadOBJ( modelFilepath );

			// Add it to the list
			g_models.push_back( pModel );
			g_modelMap[modelName] = pModel;
		}

        return true;
	}

	void Uninit() {
		for (auto pModel : g_models) {
			delete pModel;
		}
	}

	CModel *GetModel(const std::string &modelName) {
		if (g_modelMap.find(modelName) == g_modelMap.end()) {
			con_error("Could not find model %s", modelName.c_str());
			return nullptr;
		}
		return g_modelMap[modelName];
	}

	CModel::CModel()
	{
		m_nAnimationTick = 0;

		glGenVertexArrays(1, &m_nVAO);
		glGenBuffers(1, &m_nVBO);
		glGenBuffers(1, &m_nEBO);
		
		glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nEBO);
		glBindVertexArray(m_nVAO);

		// Positon
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		// Normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
		glEnableVertexAttribArray(1);
		// Texture Coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
		glEnableVertexAttribArray(2);
		// Vertex Color
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glEnableVertexAttribArray(3);

		glBindVertexArray(0);
	}

	CModel::~CModel()
	{
	}

	void CModel::Update() {
		glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_faces.size() * sizeof(Face), &m_faces[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void CModel::Render( Vector3f pos, Vector3f rot, Vector3f scale ) {
		if (m_vertices.size() == 0 || m_faces.size() == 0)
			return;
		

		m_pShader->Bind();

		glm::mat4 model = glm::mat4(1.0f);
		model			= glm::translate( model, { pos.x, pos.y, pos.z } );
		model			= glm::scale( model, { scale.x, scale.y, scale.z } );
		model			= glm::rotate( glm::rotate( glm::rotate( model, (float)( rot.z * DEG2RAD ), { 0, 0, -1 } ),
											(float)( rot.y * DEG2RAD ), { 0, -1, 0 } ),
							(float)( rot.x * DEG2RAD ), { 1, 0, 0 } );

		m_pShader->SetMat4( "model", model );
		// m_pShader->SetMat3( "normalMat", glm::mat3( glm::transpose( glm::inverse( model ) ) ) );

		m_pTexture->Bind();

		glBindVertexArray( m_nVAO );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_nEBO );
		glDrawElements( GL_TRIANGLES, m_faces.size() * sizeof(Face), GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		m_pTexture->Unbind();
		m_pShader->Unbind();
	}

	void CModel::LoadOBJ( const std::string &filePath ) {
		std::string mdlfp = "/assets/models/" + filePath;

		bool succeed;
		int64_t l;
		char *file = (char *)fileSystem::LoadFile( mdlfp.c_str(), l, succeed );

		if ( !succeed )
		{
			delete file;
			con_error( "Failed to load model file: %s", filePath.c_str() );
			return; // TODO:
		}

		char *token;
		char *saveptr;
		const char sep[2] = "\n";
		token			  = strtok_r( file, sep, &saveptr );

		std::vector<Vector3f> vertPositions;
		std::vector<Vector3f> uvCoords;
		std::vector<Vector3f> normals;

		while ( token != NULL )
		{
			// Skip Comments
			if ( token[0] == '#' )
				goto skip;

			// Vertex
			if ( token[0] == 'v' )
			{
				if ( token[1] == ' ' )
				{
					Vector3f v;
					sscanf( token, "v %f %f %f", &v.x, &v.y, &v.z );

					// printf("Vert: %f,%f,%f\n", v.x, v.y, v.z);
					vertPositions.push_back( v );
					m_vertices.push_back( {} );
				}
				else if ( token[1] == 't' )
				{
					Vector3f uv;
					sscanf( token, "vt %f %f", &uv.x, &uv.y );

					uvCoords.push_back( uv );
				}
				else if ( token[1] == 'n' )
				{
					Vector3f n;
					sscanf( token, "vn %f %f %f", &n.x, &n.y, &n.z );

					normals.push_back( n );
				}
			}
			// Face
			else if ( token[0] == 'f' )
			{
				// f vert/vertNormal/vertTexCoord vert/vertNormal/vertTexCoord vert/vertNormal/vertTexCoord

				// We actually duplicate vertices, so we can use the same index for multiple vertices
				// This is because we can't have multiple normals per vertex, so we have to duplicate vertices
				// to get the same vertex with multiple normals.

				int v[3];
				int vn[3];
				int vt[3];

				sscanf( token, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2],
						&vn[2] );

				// if (v[0] > vertPositions.size() || v[1] > vertPositions.size() || v[2] > vertPositions.size())
				// {
				// 	con_error("Error: Vertex index out of bounds!");
				// 	return;
				// }

				// con_info("Face: %d,%d,%d\n", v[0], v[1], v[2]);

				// Add the vertices
				for ( int i = 0; i < 3; i++ )
				{
					// Create a new vertex
					m_vertices.push_back( { vertPositions[v[i] - 1].x, vertPositions[v[i] - 1].y,
											vertPositions[v[i] - 1].z, normals[vn[i] - 1].x, normals[vn[i] - 1].y,
											normals[vn[i] - 1].z, uvCoords[vt[i] - 1].x, uvCoords[vt[i] - 1].y } );
				}

				m_faces.push_back(
					{ (vertexIndex_t)m_vertices.size() - 3, (vertexIndex_t)m_vertices.size() - 2, (vertexIndex_t)m_vertices.size() - 1 } );
			}
			// Bobj Extensions start here
			// Bones
			else if ( token[0] == 'b' )
			{
				// b parent x/y/z
				boneIndex_t parent;

				float x, y, z;

				sscanf( token, "b %d %f/%f/%f", &parent, &x, &y, &z );

				m_bones.push_back( { (boneIndex_t)m_bones.size(), parent, x, y, z } );
			}
			// Texture Name
			else if ( token[0] == 't' )
			{
				char *textureName = new char[256];
				sscanf( token, "t %s", textureName );

				m_pTexture = LoadTexture( textureName );

				delete[] textureName;
			}
			// Shader Name
			else if ( token[0] == 's' )
			{
				char *shaderName = new char[256];
				sscanf( token, "s %s", shaderName );

				m_pShader = GetNamedShader( shaderName );

				delete[] shaderName;
			}
			else {
				con_error( "Unknown OBJ token: %s", token );
			}

		skip:
			token = strtok_r( NULL, sep, &saveptr );
		}

		Update();
	}

	std::string CModel::DumpOBJ()
	{
		std::stringstream ss;

		// Add a header
		ss << "# VoxelThingYeah OBJ Dump\n";
		ss << "# Usability not garunteed\n";
		ss << "#\n";

		// Some other stuff
		ss << "o VoxelThingYeah-Dump\n";
		// Don't wanna have to deal with materials
		ss << "usemtl none\n";
		ss << "s off\n";

		for ( int i = 0; i < m_vertices.size(); i++ )
		{
			ss << "v " << m_vertices[i].x << " " << m_vertices[i].y << " " << m_vertices[i].z << "\n";
			ss << "vn " << m_vertices[i].nx << " " << m_vertices[i].ny << " " << m_vertices[i].nz << "\n";
			ss << "vt " << m_vertices[i].u << " " << m_vertices[i].v << "\n";
		}

		for ( int i = 0; i < m_bones.size(); i++ )
		{
			ss << "b " << m_bones[i].parent << " " << m_bones[i].x << " " << m_bones[i].y << " " << m_bones[i].z << "\n";
		}

		for ( int i = 0; i < m_faces.size(); i++ )
		{
			ss << "f " << m_faces[i].v1 + 1 << "/" << m_faces[i].v1 + 1 << "/" << m_faces[i].v1 + 1 << " " << m_faces[i].v2 + 1
			   << "/" << m_faces[i].v2 + 1 << "/" << m_faces[i].v2 + 1 << " " << m_faces[i].v3 + 1 << "/" << m_faces[i].v3 + 1
			   << "/" << m_faces[i].v3 + 1 << "\n";
		}

		return ss.str();
	}
}