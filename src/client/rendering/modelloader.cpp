#include "filesystem.hpp"

#include "logging.hpp"

#include "errormdl.hpp"
#include "modelloader.hpp"

#include <iostream>

#include <cstring>

#include "vector.hpp"

void BOBJLoadModel( CModel *m, const char *fp )
{
	bool succeed;
	int64_t l;
	char *file = (char *)fileSystem::LoadFile( fp, l, succeed );

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

	std::vector<CVector> vertPositions;
	std::vector<CVector> uvCoords;
	std::vector<CVector> normals;

	while ( token != NULL )
	{
		char *linesaveptr;
		char *lineToken = strtok_r( token, " ", &linesaveptr );

		// Skip Comments
		if ( lineToken[0] == '#' )
			goto skip;

		// Vertex
		if ( strcmp( lineToken, "v" ) == 0 )
		{
			// v x y z
			// Let's try and absorb those next 3 numbers

			// Bump it, removing the 'v'
			lineToken = strtok_r( NULL, " ", &linesaveptr );
			int i	  = 0;

			CVector v;
			v.x		  = strtof( lineToken, NULL );
			lineToken = strtok_r( NULL, " ", &linesaveptr );
			v.y		  = strtof( lineToken, NULL );
			lineToken = strtok_r( NULL, " ", &linesaveptr );
			v.z		  = strtof( lineToken, NULL );
			lineToken = strtok_r( NULL, " ", &linesaveptr );

			// printf("Vert: %f,%f,%f\n", v.x, v.y, v.z);
			vertPositions.push_back( v );
			m->m_vertices.push_back( {} );
		}
		// Vertex Tex(?), U/V Coordinate!!
		else if ( strcmp( lineToken, "vt" ) == 0 )
		{
			// vt x y
			// Let's try and absorb those next 2 numbers

			// Bump it, removing the 'vt'
			lineToken = strtok_r( NULL, " ", &linesaveptr );

			CVector uv;
			uv.x	  = strtof( lineToken, NULL );
			lineToken = strtok_r( NULL, " ", &linesaveptr );
			uv.y	  = strtof( lineToken, NULL );

			uvCoords.push_back( uv );
		}
		// Vertex Normal
		else if ( strcmp( lineToken, "vn" ) == 0 )
		{
			// vn x y z
			// Let's try and absorb those next 3 numbers

			// Bump it, removing the 'vn'
			lineToken = strtok_r( NULL, " ", &linesaveptr );
			int i	  = 0;

			CVector v;
			v.x		  = strtof( lineToken, NULL );
			lineToken = strtok_r( NULL, " ", &linesaveptr );
			v.y		  = strtof( lineToken, NULL );
			lineToken = strtok_r( NULL, " ", &linesaveptr );
			v.z		  = strtof( lineToken, NULL );
			lineToken = strtok_r( NULL, " ", &linesaveptr );

			normals.push_back( v );
		}
		// Face
		else if ( strcmp( lineToken, "f" ) == 0 )
		{
			// f vert/vertNormal/vertTexCoord vert/vertNormal/vertTexCoord vert/vertNormal/vertTexCoord

			// Bump it, removing the 'f'
			lineToken = strtok_r( NULL, " ", &linesaveptr );

			CModel::Face f;
			int idxs[3] = { 0, 0, 0 };
			int i		= 0;

			while ( lineToken != NULL )
			{
				// con_info(lineToken);

				char *segPtr;
				char *seg = strtok_r( lineToken, "/", &segPtr );

				int vertIndex		= atoi( seg ) - 1;
				seg					= strtok_r( NULL, "/", &segPtr );
				int vertNormalIndex = atoi( lineToken ) - 1;
				seg					= strtok_r( NULL, "/", &segPtr );
				int vertTexCoord	= atoi( lineToken ) - 1;

				m->m_vertices.at( vertIndex ) = { vertPositions[vertIndex].x, vertPositions[vertIndex].y,
												  vertPositions[vertIndex].z, normals[vertNormalIndex].x,
												  normals[vertNormalIndex].y, normals[vertNormalIndex].z,
												  uvCoords[vertTexCoord].x,	  uvCoords[vertTexCoord].y };
				idxs[i]						  = vertIndex;

				// printf("idx: %i\n", vertIndex);

				lineToken = strtok_r( NULL, " ", &linesaveptr );

				i++;
			}

			f.v	  = idxs[2];
			f.vv  = idxs[1];
			f.vvv = idxs[0];

			m->m_faces.push_back( f );
		}

	skip:
		token = strtok_r( NULL, sep, &saveptr );
	}

	vertPositions.clear();
	uvCoords.clear();

	m->Update();
}