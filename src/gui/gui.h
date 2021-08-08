
#include "utility/vector.h"
#include "../inputmanager.h"
#include "rendering/texturemanager.h"

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

		struct _Image {
			Texture *_tex;
			std::vector<Vertex> vertices;
		};

		std::vector<Vertex> GetQuad(Vector pos, Vector size, Colour color);
		std::vector<Vertex> GetCharQuad(const char *c, Vector pos, Vector size, Colour color);

		unsigned int vbo, vao;
		std::vector<Vertex> textVertiecs;
		std::vector<_Image> images;

		// Teh epic textTex
		Texture* textTex;
	public:
		GUI(TextureManager *texMan, int screenW, int screenH);
		~GUI();

		// Z Ignored
		Vector mousePos;

		// Used to get the input
		InputManager* inputMan;

		// Update
		void Update();

		Vector screenCentre;

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
		void Label(const char* text, Vector pos, Colour color = Color(1,1,1));
		void Image(Texture* tex, Vector pos, Vector size, Vector origin = Vector(0,0));
};