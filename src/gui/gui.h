
#include "utility/vector.h"
#include "../inputmanager.h"

#include <vector>

// Reference taken / tutorial followed
// https://sol.gfxile.net/imgui/

// OOP POO
class GUI
{
	private:
		struct Vertex {
			float x,y,z; //! Z always 0!!
			float u,v;
			// Color
			float r,g,b;
		};

		std::vector<Vertex> GetQuad(Vector pos, Vector size, Colour color);

		unsigned int vbo, vao;
		std::vector<Vertex> Vertices;
	public:
		GUI();
		~GUI();

		// Z Ignored
		Vector mousePos;

		// Used to get the input
		InputManager* inputMan;

		// Update
		void Update();

		// Current button pressed down
		int mouseState;
		// Item under the mouse
		int hotItem;
		// Current interaction
		int activeItem;

		// Z Ignored!
		bool RegionHit(Vector pos, Vector size);

		// Elements
		int Button(int id, Vector pos, Vector size);
};