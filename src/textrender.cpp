#include "vector.h"

#include <glad/glad.h>

#include "shadermanager.h"
#include "texturemanager.h"

#include "textrender.h"

#include <map>
#include <vector>
#include <string>

// const std::map<std::string, std::vector<float>> GlyphUV[128] = {
// 	{"A", {0.0f,0.0f, 1.0f,1.0f}},
// 	{"B", {1.0f,0.0f, 1.0f,1.0f}},
// 	{"C", {2.0f,0.0f, 1.0f,1.0f}},
// 	{"D", {3.0f,0.0f, 1.0f,1.0f}},
// 	{"E", {4.0f,0.0f, 1.0f,1.0f}},
// 	{"F", {5.0f,0.0f, 1.0f,1.0f}},
// 	{"G", {6.0f,0.0f, 1.0f,1.0f}},
// 	{"H", {7.0f,0.0f, 1.0f,1.0f}},
	
// 	{"I", {0.0f,1.0f, 1.0f,1.0f}},
// 	{"J", {1.0f,1.0f, 1.0f,1.0f}},
// 	{"K", {2.0f,1.0f, 1.0f,1.0f}},
// 	{"L", {3.0f,1.0f, 1.0f,1.0f}},
// 	{"M", {4.0f,1.0f, 1.0f,1.0f}},
// 	{"N", {5.0f,1.0f, 1.0f,1.0f}},
// 	{"O", {6.0f,1.0f, 1.0f,1.0f}},
// 	{"P", {7.0f,1.0f, 1.0f,1.0f}},

// 	{"Q", {0.0f,2.0f, 1.0f,1.0f}},
// 	{"R", {1.0f,2.0f, 1.0f,1.0f}},
// 	{"S", {2.0f,2.0f, 1.0f,1.0f}},
// 	{"T", {3.0f,2.0f, 1.0f,1.0f}},
// 	{"U", {4.0f,2.0f, 1.0f,1.0f}},
// 	{"V", {5.0f,2.0f, 1.0f,1.0f}},
// 	{"W", {6.0f,2.0f, 1.0f,1.0f}},
// 	{"X", {7.0f,2.0f, 1.0f,1.0f}},

// 	{"Y", {6.0f,3.0f, 1.0f,1.0f}},
// 	{"Z", {7.0f,3.0f, 1.0f,1.0f}},
// };

namespace TextRendering
{

	unsigned int textVao;
	unsigned int textVbo;

	void InitText()
	{
		glGenVertexArrays(1, &textVao);
		glGenBuffers(1, &textVbo);

		glBindVertexArray(textVao);
		
		glBindBuffer(GL_ARRAY_BUFFER, textVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);

		// Position
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture coordinate
		// glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		// glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	void RenderText(const char* text, Vector pos, TextureManager* texman)
	{
		// Shader textShader("shaders/generic.vert", "shaders/text.frag");
		// Texture textTexture = texman->LoadTexture("font.png");

		pos = pos + Vector(0.5, 0.5);

		const float w = 8.0f / 128.0f;
		const float h = 9.0f / 128.0f;

		float x = pos.x;
		float y = pos.y;

		// glBindTexture(GL_TEXTURE_2D, textTexture.id);
		// glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(textVao);

		for (int i = 0; i < sizeof(text); i++)
		{
			char c = text[i];

			float verticies[6][5] = {
				{ x + w, y,     0,  1.0f, 1.0f },
				{ x,     y,     0,  0.0f, 1.0f },
				{ x,     y + h, 0,  0.0f, 0.0f },

				{ x + w, y + h, 0,  1.0f, 0.0f },
				{ x + w, y,     0,  1.0f, 1.0f },
				{ x,     y + h, 0,  0.0f, 0.0f },
			};

			glBindBuffer(GL_ARRAY_BUFFER, textVbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticies), verticies);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			
			x += w;		
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};