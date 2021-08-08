// An ImGui
// TODO: Abstract OpenGL away from this, making it renderer agnostic
//       (Store like a mesh, have some kind of renderer)

#include "gui.h"

#include <glad/glad.h>

// The whole coordinate system is based on these GUIUUNITs.
// When we start to dynamically rescale the GUI based on resolution this will come in handy. for now, 16px.
#define GUIUNIT 16

#define TEXTINTEXWIDTH 8
#define TEXTINTEXHEIGHT 9
#define TEXTRATIO 1.125

#define TEXTWIDTH GUIUNIT * TEXTRATIO
#define TEXTHEIGHT GUIUNIT

// I am not supporting weird configurations
#define TEXTTILES 16

GUI::GUI(TextureManager *texMan, int screenW, int screenH) :
	screenCentre((screenW * 0.5) / GUIUNIT, (screenH * 0.5) / GUIUNIT),
	mouseState(IN_NO_MOUSE),
	activeItem(0),
	hotItem(0)
{	
	// OpenGl
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 8, NULL, GL_DYNAMIC_DRAW);


		// + sizeof(Texture*) so we skip the little internal thing at the end

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
		glBindVertexArray(vao);
		// Text
		{
			glBindTexture(GL_TEXTURE_2D, textTex->id);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, textVertiecs.size() * sizeof(GUI::Vertex), textVertiecs.data(), GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES, 0, (textVertiecs.size() * sizeof(GUI::Vertex)) / 6);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		// Images
		{
			for (GUI::_Image &img : images)
			{
				glBindTexture(GL_TEXTURE_2D, img._tex->id);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, img.vertices.size() * sizeof(GUI::Vertex), img.vertices.data(), GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glDrawArrays(GL_TRIANGLES, 0, (img.vertices.size() * sizeof(GUI::Vertex)) / 6);
				glBindTexture(GL_TEXTURE_2D, 0);				
			}
		}
		glBindVertexArray(0);
	}


	// Clear our vertices
	textVertiecs.clear();
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
	x = (int(*c - ' ') % TEXTTILES) * TEXTINTEXWIDTH;
	y = (int(*c - ' ') / TEXTTILES) * TEXTINTEXHEIGHT;

	vert[0].u = (x + TEXTINTEXWIDTH) / (16.0f * TEXTINTEXWIDTH);
	vert[0].v = (y + TEXTINTEXHEIGHT) / (16.0f * TEXTINTEXHEIGHT);

	vert[1].u = (x) / (16.0f * TEXTINTEXWIDTH);
	vert[1].v = (y + TEXTINTEXHEIGHT) / (16.0f * TEXTINTEXHEIGHT);

	vert[2].u = (x) / (16.0f * TEXTINTEXWIDTH);
	vert[2].v = (y) / (16.0f * TEXTINTEXHEIGHT);


	vert[3].u = (x + TEXTINTEXWIDTH) / (16.0f * TEXTINTEXWIDTH);
	vert[3].v = (y) / (16.0f * TEXTINTEXHEIGHT);

	vert[4].u = (x + TEXTINTEXWIDTH) / (16.0f * TEXTINTEXWIDTH);
	vert[4].v = (y + TEXTINTEXHEIGHT) / (16.0f * TEXTINTEXHEIGHT);

	vert[5].u = (x) / (16.0f * TEXTINTEXWIDTH);
	vert[5].v = (y) / (16.0f * TEXTINTEXHEIGHT);

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

	pos = pos * GUIUNIT;
	size = size * GUIUNIT;

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
		std::copy(g.begin(), g.end(), std::back_inserter(textVertiecs));
	}

	return returnCode;
}

void GUI::Label(const char* text, Vector pos, Colour color)
{
	pos = pos * GUIUNIT;

	// Render
	// OpenGl
	{
		int i = 0;
		
		while(text[i] != '\0')
		{
			// Get vertices
			std::vector<GUI::Vertex> g = GetCharQuad(&text[i], pos, Vector(TEXTWIDTH, TEXTHEIGHT), color);
			std::copy(g.begin(), g.end(), std::back_inserter(textVertiecs));

			pos = pos + Vector(TEXTWIDTH,0);

			i++;
		}
	}
}

void GUI::Image(Texture* tex, Vector pos, Vector size, Vector origin)
{
	pos = pos * GUIUNIT;
	size = size * GUIUNIT;

	{
		GUI::_Image img;
		img.vertices = GetQuad(pos - (size * origin), size, Colour(1,1,1));
		img._tex = tex;
		images.push_back(img);
	}
}