// An ImGui
// TODO: Abstract OpenGL away from this, making it renderer agnostic
//       (Store like a mesh, have some kind of renderer)

#include "gui.h"

#include <glad/glad.h>

GUI::GUI()
{
	// OpenGl
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);

		// Position
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture coordinate
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}
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
	mousePos = inputMan->mousePos;
	mouseDown = inputMan->mouseState;

	hotItem = 0;

	// Render
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}


	// Clear our vertices
	Vertices.clear();
}

std::vector<GUI::Vertex> GUI::GetQuad(float x, float y, float w, float h)
{
	return {
		{ x + w, y,     0,  1.0f, 1.0f },
		{ x,     y,     0,  0.0f, 1.0f },
		{ x,     y + h, 0,  0.0f, 0.0f },

		{ x + w, y + h, 0,  1.0f, 0.0f },
		{ x + w, y,     0,  1.0f, 1.0f },
		{ x,     y + h, 0,  0.0f, 0.0f },
	};
}

bool GUI::RegionHit(Vector pos, Vector size)
{
	if (
		mousePos < pos ||
		mousePos > pos + size
	)
		return false;
	return true;
}

int GUI::Button(int id, Vector pos, Vector size)
{
	int returnCode = 0;

	// Check & set State
	if (RegionHit(pos, size))
	{
		hotItem = id;
		if (activeItem == 0 && mouseDown)
			activeItem = id;
	}

	if (mouseDown == 0 && hotItem == id && activeItem == id)
		returnCode = 1;

	// Render
	// OpenGl
	{
		// Get vertices
		std::vector<GUI::Vertex> g = GetQuad(pos.x, pos.x, size.x, size.y);
		std::copy(g.begin(), g.end(), std::back_inserter(Vertices));

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(GUI::Vertex), Vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	return returnCode;
}