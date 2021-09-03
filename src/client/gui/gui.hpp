
#include "utility/vector.hpp"
#include "shared/inputmanager.hpp"
#include "rendering/texturemanager.hpp"
#include "rendering/shadermanager.hpp"

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
			Texture *_tex = nullptr;
			std::vector<Vertex> vertices;
		};

		std::vector<Vertex> GetQuad(Vector pos, Vector size, Colour color, Vector uStart = Vector(0,0), Vector uEnd = Vector(1,1));
		std::vector<Vertex> GetCharQuad(const char *c, Vector pos, Vector size, Colour color);

		unsigned int vbo, vao;
		std::vector<Vertex> textVertiecs;
		std::vector<_Image> images;

		// Teh epic textTex
		Texture* textTex = nullptr;
		Shader *textShader = nullptr;
	public:
		GUI(int screenW, int screenH);
		~GUI();

		// TODO: Put this in a generic utility class and then make all atlas-based things (i.e text, blocks) inherit it
		struct Atlas
		{
			Atlas(float x, float y, float sizex, float sizey)
			{
				this->x=x;
				this->y=y;
				this->sizex=sizex;
				this->sizey=sizey;
			};
			float x, y, sizex, sizey;
		};

		// Z Ignored
		Vector mousePos;

		// Used to get the input
		InputManager* inputMan = nullptr;

		// Update
		void Update();

		void Resize(int x, int y);

		Vector screenCentre;
		Vector screenDimensions;

		// Current button pressed down
		int mouseState;
		// Item under the mouse
		int hotItem;
		// Current interaction
		int activeItem;

		// Z Ignored!
		bool RegionHit(Vector pos, Vector size);

		// Converts positions to screen ones, including negatives being from the opposite side and stuff
		Vector GetInScreen(Vector pos);

		// Elements
		int Button(int id, Vector pos, Vector size);
		void Label(const char* text, Vector pos, Colour color = Color(1,1,1));
		void Image(Texture* tex, Vector pos, Vector size, Vector origin = Vector(0,0));
		void ImageAtlas(Texture* tex, Atlas atlas, float atlasDivisions, Vector pos, Vector size, Vector origin = Vector(0,0));
};