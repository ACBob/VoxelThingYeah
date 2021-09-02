#include "filesystem.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

#include "modelloader.hpp"

#include <iostream>

#include <cstring>

#include "vector.hpp"

void LoadModel(Model &m, const char *fp)
{
	bool succeed;
	int64_t l;
	char *file = (char*)fileSystem::LoadFile(fp, l, succeed);

	if (!succeed)
		return;
	

	char *token;
	char *saveptr;
	const char sep[2] = "\n";
	token = strtok_r(file, sep, &saveptr);

	bool comment = false;

	std::vector<Vector> vertPositions;

	while (token != NULL)
	{
		char *linesaveptr;
		char *lineToken = strtok_r(token, " ", &linesaveptr);

		// Skip Comments
		if (lineToken[0] == '#')
			goto skip;

		if (strcmp(lineToken, "v") == 0)
		{
			// v x y z
			// Let's try and absorb those next 3 numbers

			// Bump it, removing the 'v'
			lineToken = strtok_r(NULL, " ", &linesaveptr);
			int i = 0;

			Vector v;
			v.x = strtof(lineToken, NULL);
			lineToken = strtok_r(NULL, " ", &linesaveptr);
			v.y = strtof(lineToken, NULL);
			lineToken = strtok_r(NULL, " ", &linesaveptr);
			v.z = strtof(lineToken, NULL);
			lineToken = strtok_r(NULL, " ", &linesaveptr);

			printf("Vert: %f,%f,%f\n", v.x, v.y, v.z);
			vertPositions.push_back(v);
			m.vertices.push_back({});
		}
		else if (strcmp(lineToken, "f") == 0)
		{
			// f vert/vertNormal/vertTexCoord vert/vertNormal/vertTexCoord vert/vertNormal/vertTexCoord
			
			// Bump it, removing the 'f'
			lineToken = strtok_r(NULL, " ", &linesaveptr);

			Model::Face f;
			int idxs[3] = {0,0,0};
			int i = 0;

			while (lineToken != NULL)
			{
				con_info(lineToken);

				char *segPtr;
				char *seg = strtok_r(lineToken, "/", &segPtr);

				int vertIndex = atoi(seg) - 1;
				seg = strtok_r(NULL, "/", &segPtr);
				// int vertNormalIndex = atoi(lineToken);
				seg = strtok_r(NULL, "/", &segPtr);
				// int vertTexCoord = atoi(lineToken);

				m.vertices.at(vertIndex) =
					{
						vertPositions[vertIndex].x, 
						vertPositions[vertIndex].y, 
						vertPositions[vertIndex].z,
						0,0,
						0,0
					};
				idxs[i] = vertIndex;
				
				printf("idx: %i\n", vertIndex);

				lineToken = strtok_r(NULL, " ", &linesaveptr);

				i ++;
			}

			f.v = idxs[2];
			f.vv = idxs[1];
			f.vvv = idxs[0];

			m.faces.push_back(f);
		}


		skip:
		token = strtok_r(NULL, sep, &saveptr);
	}

	m.Update();
}