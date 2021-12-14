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

#include <cstring>

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
		else {
			con_error( "Unknown OBJ token: %s", token );
		}

	skip:
		token = strtok_r( NULL, sep, &saveptr );
	}

	Update();
}