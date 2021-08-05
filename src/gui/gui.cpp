// An ImGui
// TODO: Abstract OpenGL away from this, making it renderer agnostic
//       (Store like a mesh, have some kind of renderer)

#include "gui.h"

#include <glad/glad.h>

GUI::GUI()
{
	mouseState = InputManager::MouseState::NONE;
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
	mouseState = inputMan->mouseState;

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
		if (activeItem == 0 && (mouseState & InputManager::MouseState::LEFT) != 0)
		{
			activeItem = id;
		}
	}

	if (mouseState == 0 && hotItem == id && activeItem == id)
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

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(GUI::Vertex), Vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	return returnCode;
}