
#include "rendering/shadermanager.hpp"
#include "rendering/texturemanager.hpp"
#include "shared/inputmanager.hpp"
#include "shared/statemanager.hpp"
#include "utility/vector.hpp"

#include "colour.hpp"

#include <map>
#include <vector>

#pragma once

// Reference taken / tutorial followed
// https://sol.gfxile.net/imgui/

#ifdef GUI_SRC_ID
	#define GUIGEN_ID ( ( IMGUI_SRC_ID ) + ( __LINE__ ) )
#else
	#define GUIGEN_ID ( __LINE__ )
#endif

using GuiID = unsigned int;

// TODO: Font from TOML

// OOP POO
class CGui
{
	private:
		// struct FontDef {
		// 	std::vector<CTexture*> m_pTextures;
		// 	std::vector<wchar_t> characters;
		// 	int m_iFontWidths[256];
		// };

		unsigned int m_iVao;
		unsigned int m_iVbo;

		struct GuiVert {
			float x, y, z;
			float u, v;
			float r, g, b, a;
		};

		struct GuiImage {
			CTexture *pTex;
			std::vector<GuiVert> verts;
		};

		// NOTE: Z is ignored for size, but it might be interesting to see for a 3D GUI?
		std::vector<GuiVert> GetRect( Vector3f pos, Vector3f size, Vector4f uv, CColour colour );

		std::vector<GuiVert> m_vertices;
		std::vector<GuiImage> m_images;

		// Renders a generic quad here, but under image
		void _Image( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint, Vector4f uv = {0, 0, 1, 1} );
		void _9PatchRect( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint, float borderSize );
		float _TextLength( const char *text, float scale = 1.0f );
	public:
		CGui(Vector3f size);
		~CGui();

		void Resize( Vector3f screenDimensions );
		void Update();

		// true if the mouse is in this position
		// TODO: how to handle overlapping elements? (maybe test the z values?)
		bool RegionHit( Vector3f vPosition, Vector3f vSize );

		Vector3f GetInScreen( Vector3f pos );

		// Current button pressed down
		int m_iMouseState;
		// Item under the mouse
		int m_iHotItem;
		// Current interaction
		int m_iActiveItem;
		// Current element with keyboard element
		int m_iKeyboardItem;
		// Position of the mouse
		Vector3i m_vMousePos;

		// Size in pixels of one grid unit
		int m_iGUIUnitSize = 16;

		CTexture *m_pFontTex = nullptr;
		float m_charWidths[256];

		CShader *m_pShader = nullptr;

		// Some textures we just hold because they're related to the GUI
		CTexture *m_pButtonTex = nullptr;

		CInputManager *m_pInputManager;

		Vector3f m_vScreenDimensions;
		Vector3f m_vScreenCentre;

		// Chooses the alignment of text
		enum TextAlignment {
			TEXTALIGN_LEFT,   // |TEXT  |
			TEXTALIGN_CENTER, // | TEXT |
			TEXTALIGN_RIGHT   // |  TEXT|
		};

		// Base Elements
		void Image( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint = {255, 255, 255} );
		bool Button( GuiID id, Vector3f position, Vector3f minSize = {4, 2}, CTexture *pTexture = nullptr );
		void Label( const char* text, Vector3f position, float scale = 1.0f, CColour colour = {255, 255, 255}, TextAlignment alignment = TEXTALIGN_LEFT );

		// Composite Elements
		bool LabelButton( GuiID id, const char* text, Vector3f position, Vector3f size, TextAlignment alignment = TEXTALIGN_LEFT );

		// Function Graveyard
		// (I am about to remove these, they are just stubbed so I can test others)
		using Atlas = Vector4f;
		CTexture *m_pBGTex = nullptr;
		int m_iGuiUnit = 0;
		void SetTextBuffer(...) {};
		const char* GetTextBuffer(...) { return ""; };
		void ClearBuffers() {};
		int AtlasButton( int id, CTexture *tex, Atlas atlas, float atlasDivisions, Vector3f pos, Vector3f size,
					 Vector3f vOrigin = Vector3f( 0, 0 ) ) {};
		void Image( CTexture *pTex, Vector3f vPosition, Vector3f vSize, Vector3f vOrigin = Vector3f( 0, 0 ),
					CColour tint = CColour( 255, 255, 255 ) ) {};
		void ImageAtlas( CTexture *pTex, Atlas atlas, float fAtlasDivisions, Vector3f vPosition, Vector3f vSize,
						Vector3f vOrigin = Vector3f( 0, 0 ), CColour tint = CColour( 255, 255, 255 ) );
		void Image9Rect( CTexture *pTex, Vector3f pos, Vector3f size, CColour color ) {};
		void Crosshair() {}; // A wrapper for Image
		const char *TextInput( int iId, Vector3f vPosition ) {};
		const char *SelectableTextInput( int id, Vector3f pos, Vector3f size,
										CTexture *pTex = nullptr ) {}; // Variant of TextInput that has support for selection,
																	// also rendering with a background
		bool Slider( int id, Vector3f pos, Vector3f size, int max, int &value ) {};
		bool HorzSlider( int id, Vector3f pos, Vector3f size, int max, int &value ) {}; // Same as Slider but horizontal
		bool CheckBox( int id, Vector3f pos, Vector3f size, bool &value ) {};
};