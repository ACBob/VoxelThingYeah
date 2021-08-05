#include <glad/glad.h>

#include "textrender.h"

#include <map>
#include <vector>
#include <string>

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