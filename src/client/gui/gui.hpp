
#include "rendering/shadermanager.hpp"
#include "rendering/texturemanager.hpp"
#include "shared/inputmanager.hpp"
#include "utility/vector.hpp"

#include <map>
#include <vector>

// Reference taken / tutorial followed
// https://sol.gfxile.net/imgui/

// OOP POO
class CGui
{
  private:
	struct Vertex
	{
		float x, y, z; //! Z always 0!!
		float u, v;
		// Color
		float r, g, b;
	};

	struct _Image
	{
		CTexture *_tex = nullptr;
		std::vector<Vertex> vertices;
	};

	std::vector<Vertex> GetQuad( CVector pos, CVector size, Colour color, CVector uStart = CVector( 0, 0 ),
								 CVector uEnd = CVector( 1, 1 ) );
	std::vector<Vertex> GetCharQuad( const char *c, CVector pos, CVector size, Colour color );

	unsigned int vbo, vao;
	std::vector<Vertex> textVertiecs;
	std::vector<_Image> images;

	// Teh epic textTex
	CTexture *textTex   = nullptr;
	CShader *textShader = nullptr;

	std::map<int, std::string> textBuffers;

  public:
	CGui( int screenW, int screenH );
	~CGui();

	// TODO: Put this in a generic utility class and then make all atlas-based things (i.e text, blocks) inherit it
	struct Atlas
	{
		Atlas( float x, float y, float sizex, float sizey )
		{
			this->x		= x;
			this->y		= y;
			this->sizex = sizex;
			this->sizey = sizey;
		};
		float x, y, sizex, sizey;
	};

	// Z Ignored
	CVector mousePos;

	// Used to get the input
	CInputManager *inputMan = nullptr;

	// Update
	void Update();

	void Resize( int x, int y );

	CVector screenCentre;
	CVector screenDimensions;

	// Current button pressed down
	int mouseState;
	// Item under the mouse
	int hotItem;
	// Current interaction
	int activeItem;

	// Z Ignored!
	bool RegionHit( CVector pos, CVector size );

	// Converts positions to screen ones, including negatives being from the opposite side and stuff
	CVector GetInScreen( CVector pos );

	// Elements
	int Button( int id, CVector pos, CVector size );
	void Label( const char *text, CVector pos, Colour color = Color( 1, 1, 1 ) );
	void Image( CTexture *tex, CVector pos, CVector size, CVector origin = CVector( 0, 0 ) );
	void ImageAtlas( CTexture *tex, Atlas atlas, float atlasDivisions, CVector pos, CVector size,
					 CVector origin = CVector( 0, 0 ) );
	const char *TextInput( int id, CVector pos );
};