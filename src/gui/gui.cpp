// An ImGui
// TODO: Abstract OpenGL away from this, making it renderer agnostic
//       (Store like a mesh, have some kind of renderer)

#include "gui.h"

#include <glad/glad.h>

// TODO: not this
#define TEXTWIDTH 8
#define TEXTHEIGHT 9

// I am not supporting weird configurations
#define TEXTTILES 16

GUI::GUI(TextureManager *texMan)
{
	mouseState = IN_NO_MOUSE;
	activeItem = 0;
	hotItem = 0;


	// OpenGl
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 8, NULL, GL_DYNAMIC_DRAW);

		// Position
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture coordinate
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)offsetof(GUI::Vertex, u));
		glEnableVertexAttribArray(1);
		// colour
		glVertexAttribPointer(2, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)offsetof(GUI::Vertex, r));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	textTex = texMan->LoadTexture("font.png");
}

GUI::~GUI()
{
	// OpenGl
	{
		glDeleteBuffers(1, &vao);
		glDeleteVertexArrays(1, &vbo);
	}
}

void GUI::Update()
{
	if (mouseState == IN_NO_MOUSE)
		activeItem = 0;
	
	mousePos = inputMan->mousePos;
	mouseState = inputMan->mouseState;
	// else if (activeItem == 0)
	// 	activeItem = -1;

	hotItem = 0;

	// Render
	{
		glBindTexture(GL_TEXTURE_2D, textTex->id);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(GUI::Vertex), Vertices.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, (Vertices.size() * sizeof(GUI::Vertex)) / 6);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}


	// Clear our vertices
	Vertices.clear();
}

std::vector<GUI::Vertex> GUI::GetQuad(Vector pos, Vector size, Colour color)
{
	return {
		{ pos.x + size.x, pos.y,          0,  1.0f, 1.0f,  color.x,color.y,color.z },
		{ pos.x,          pos.y,          0,  0.0f, 1.0f,  color.x,color.y,color.z },
		{ pos.x,          pos.y + size.y, 0,  0.0f, 0.0f,  color.x,color.y,color.z },

		{ pos.x + size.x, pos.y + size.y, 0,  1.0f, 0.0f,  color.x,color.y,color.z },
		{ pos.x + size.x, pos.y,          0,  1.0f, 1.0f,  color.x,color.y,color.z },
		{ pos.x,          pos.y + size.y, 0,  0.0f, 0.0f,  color.x,color.y,color.z },
	};
}
std::vector<GUI::Vertex> GUI::GetCharQuad(const char* c, Vector pos, Vector size, Colour color)
{
	std::vector<GUI::Vertex> vert = GetQuad(pos, size, color);

	float x, y;
	x = (int(*c - ' ') % TEXTTILES) * TEXTWIDTH;
	y = (int(*c - ' ') / TEXTTILES) * TEXTHEIGHT;

	vert[0].u = (x + TEXTWIDTH) / (16.0f * TEXTWIDTH);
	vert[0].v = (y + TEXTHEIGHT) / (16.0f * TEXTHEIGHT);

	vert[1].u = (x) / (16.0f * TEXTWIDTH);
	vert[1].v = (y + TEXTHEIGHT) / (16.0f * TEXTHEIGHT);

	vert[2].u = (x) / (16.0f * TEXTWIDTH);
	vert[2].v = (y) / (16.0f * TEXTHEIGHT);


	vert[3].u = (x + TEXTWIDTH) / (16.0f * TEXTWIDTH);
	vert[3].v = (y) / (16.0f * TEXTHEIGHT);

	vert[4].u = (x + TEXTWIDTH) / (16.0f * TEXTWIDTH);
	vert[4].v = (y + TEXTHEIGHT) / (16.0f * TEXTHEIGHT);

	vert[5].u = (x) / (16.0f * TEXTWIDTH);
	vert[5].v = (y) / (16.0f * TEXTHEIGHT);

	return vert;
}

bool GUI::RegionHit(Vector pos, Vector size)
{	
	if (
		mousePos.x < pos.x ||
		mousePos.y < pos.y ||

		mousePos.x > pos.x + size.x ||
		mousePos.y > pos.y + size.y
	)
		return false;
	return true;
}

int GUI::Button(int id, Vector pos, Vector size)
{
	int returnCode = 0;

	Colour color = Colour(0.5,0.5,0.5);

	// Check & set State
	if (RegionHit(pos, size))
	{
		hotItem = id;
		color = Colour(0.75, 0.75, 0.75);
		if (mouseState != 0)
			NULL; // Breakpoint
		if (activeItem == 0 && (mouseState == IN_LEFT_MOUSE))
		{
			activeItem = id;
		}
	}

	if (mouseState == IN_NO_MOUSE && hotItem == id && activeItem == id)
	{
		returnCode = 1;
		color = Colour(0.75, 0.75, 1);
	}

	// Render
	// OpenGl
	{
		// Get vertices
		std::vector<GUI::Vertex> g = GetQuad(pos, size, color);
		std::copy(g.begin(), g.end(), std::back_inserter(Vertices));
	}

	return returnCode;
}

void GUI::Label(const char* text, Vector pos, Colour color)
{
	// Render
	// OpenGl
	{
		int i = 0;
		
		while(text[i] != '\0')
		{
			// Get vertices
			std::vector<GUI::Vertex> g = GetCharQuad(&text[i], pos, Vector(TEXTWIDTH * 4, TEXTHEIGHT * 4), color);
			std::copy(g.begin(), g.end(), std::back_inserter(Vertices));

			pos = pos + Vector(TEXTWIDTH * 4,0);

			i++;
		}
	}
}