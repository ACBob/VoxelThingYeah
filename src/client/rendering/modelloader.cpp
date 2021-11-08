#include "filesystem.hpp"

#include "logging.hpp"

#include "errormdl.hpp"
#include "modelloader.hpp"

#include <iostream>

#include <cstring>

#include "vector.hpp"

void BOBJLoadModel( CModel *m, const char *fp )
{
	char *mdlfp = new char[strlen(fp) + 16];
	strcpy(mdlfp, "/assets/models/");
	strcat(mdlfp, fp);

	bool succeed;
	int64_t l;
	char *file = (char *)fileSystem::LoadFile( mdlfp, l, succeed );

	delete[] mdlfp;

	if ( !succeed )
	{
		delete file;

		file = new char[strlen( ERRORMDLOBJ ) + 1];
		strcpy( file, ERRORMDLOBJ );
		file[strlen( ERRORMDLOBJ ) + 1] = '\0';
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
		if ( token[0] == 'v')
		{
			if (token[1] == ' ')
			{
				Vector3f v;
				sscanf( token, "v %f %f %f", &v.x , &v.y , &v.z );


				// printf("Vert: %f,%f,%f\n", v.x, v.y, v.z);
				vertPositions.push_back( v );
				m->m_vertices.push_back( {} );
			}
			else if ( token[1] == 't' )
			{
				Vector3f uv;
				sscanf( token, "vt %f %f", &uv.x , &uv.y );

				uvCoords.push_back( uv );
			}
			else if ( token[1] == 'n' )
			{
				Vector3f n;
				sscanf( token, "vn %f %f %f", &n.x , &n.y , &n.z );

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

			sscanf( token, "f %d/%d/%d %d/%d/%d %d/%d/%d",
					&v[0], &vt[0], &vn[0],
					&v[1], &vt[1], &vn[1],
					&v[2], &vt[2], &vn[2] );

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
				m->m_vertices.push_back( {
					vertPositions[v[i] - 1].x,
					vertPositions[v[i] - 1].y,
					vertPositions[v[i] - 1].z,
					normals[vn[i] - 1].x,
					normals[vn[i] - 1].y,
					normals[vn[i] - 1].z,
					uvCoords[vt[i] - 1].x,
					uvCoords[vt[i] - 1].y
				} );
			}

			m->m_faces.push_back( {
				(int)m->m_vertices.size() - 1,
				(int)m->m_vertices.size() - 2,
				(int)m->m_vertices.size() - 3
			} );
		}

	skip:
		token = strtok_r( NULL, sep, &saveptr );
	}

	vertPositions.clear();
	uvCoords.clear();

	m->Update();
}