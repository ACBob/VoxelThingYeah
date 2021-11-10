
#include "rendering/shadermanager.hpp"
#include "rendering/texturemanager.hpp"
#include "inputmanager.hpp"
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

		// Buffers
		std::map<int, std::string> m_textBuffers;
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

		int m_iTick = 0;

		// Size in pixels of one grid unit
		int m_iGUIUnitSize = 16;

		CTexture *m_pFontTex = nullptr;
		float m_charWidths[256];

		CShader *m_pShader = nullptr;

		// Some textures we just hold because they're related to the GUI
		CTexture *m_pButtonTex = nullptr;
		CTexture *m_pTextEditTex = nullptr;
		CTexture *m_pSliderTex = nullptr;
		CTexture *m_pSliderThumbTex = nullptr;
		CTexture *m_pCheckedBoxTex = nullptr;
		CTexture *m_pUnCheckedBoxTex = nullptr;

		CInputManager *m_pInputManager;

		Vector3f m_vScreenDimensions;
		Vector3f m_vScreenCentre;

		const char* GetTextBuffer(int id) {
			return m_textBuffers[id].c_str();
		}
		void SetTextBuffer(int id, const char* text) {
			m_textBuffers[id] = text;
		}
		void ClearBuffers() {
			m_textBuffers.clear();
		}

		// Chooses the alignment of text
		enum TextAlignment {
			TEXTALIGN_LEFT,   // |TEXT  |
			TEXTALIGN_CENTER, // | TEXT |
			TEXTALIGN_RIGHT   // |  TEXT|
		};

		// Base Elements
		void Image( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint = {255, 255, 255} );
		bool Button( GuiID id, Vector3f position, Vector3f minSize = {4, 2}, CTexture *pTexture = nullptr );

		// NOTE: Also generic text rendering
		void Label( const char* text, Vector3f position, float scale = 1.0f, CColour colour = {255, 255, 255}, TextAlignment alignment = TEXTALIGN_LEFT );

		const char *TextInput( int iId, Vector3f vPosition );
		bool Slider( int id, Vector3f pos, Vector3f size, int max, int &value ); // Adjustable slider
		bool HorzSlider( int id, Vector3f pos, Vector3f size, int max, int &value ); // Same as Slider but slides horizontally
		bool CheckBox( int id, Vector3f pos, bool &value ); // Top-left aligned

		// Composite Elements
		bool LabelButton( GuiID id, const char* text, Vector3f position, Vector3f size, TextAlignment alignment = TEXTALIGN_LEFT );
		const char *SelectableTextInput( int id, Vector3f pos, Vector3f size );
};