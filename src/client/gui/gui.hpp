
#include "rendering/shadermanager.hpp"
#include "rendering/texturemanager.hpp"
#include "inputmanager.hpp"
#include "shared/statemanager.hpp"
#include "utility/vector.hpp"

#include "colour.hpp"

#include <map>
#include <vector>

#include "shared/inventory/inventory.hpp"

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

		void _DrawText( const char *text, Vector3f pos, float scale, CColour colour );

		enum ButtonState {
			BUTTON_STATE_NORMAL,
			BUTTON_STATE_HOVER,
			BUTTON_STATE_PRESSED,
			BUTTON_STATE_DISABLED
		};

		ButtonState _Button( int id, Vector3f pos, Vector3f size );

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

		// *We* don't use these textures, but it makes sense for us to hold them
		CTexture *m_pGuiBGTex = nullptr;
		CTexture *m_pGuiTitleTex = nullptr;
		CTexture *m_pCrosshairTex = nullptr;
		CTexture *m_pHotbarTex = nullptr;
		CTexture *m_pHotbarSelectTex = nullptr;
		CTexture *m_pWindowTex = nullptr;
		CTexture *m_pLanguageButtonTex = nullptr;
		

		CInputManager *m_pInputManager;

		// In pixels
		Vector3f m_vScreenDimensions;
		Vector3f m_vScreenCentre;

		// In units
		Vector3f m_vGUISize;
		Vector3f m_vGUICentre;

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
		// Basic image, bottom-left centered
		void Image( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint = {255, 255, 255} );
		// Repeating image, repeats the texture where 8 pixels is a gui unit,
		void ImageRepeating( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint = {255, 255, 255} );
		// Image where the origin is the middle of the size
		void ImageCentered( Vector3f pos, Vector3f size, CTexture* pTex, CColour tint = {255, 255, 255} );

		void Image9Patch( Vector3f pos, Vector3f size, float borderRadius, CTexture* pTex, CColour tint = {255, 255, 255} );
		void Image9PatchCentered( Vector3f pos, Vector3f size, float borderRadius, CTexture* pTex, CColour tint = {255, 255, 255} );

		bool Button( GuiID id, Vector3f position, Vector3f minSize = {4, 2}, CTexture *pTexture = nullptr );
		bool ButtonCentered( GuiID id, Vector3f position, Vector3f minSize = {4, 2}, CTexture *pTexture = nullptr );

		bool Item( GuiID id, Vector3f position, Vector3f size, CItem *pItem );
		bool ItemCentered( GuiID id, Vector3f position, Vector3f size, CItem *pItem );

		int Inventory( Vector3f position, int itemsAccross, CInventory *pInventory );
		int InventoryCentered( Vector3f position, int itemsAccross, CInventory *pInventory );

		// NOTE: Also generic text rendering
		void Label( const char* text, Vector3f position, float scale = 1.0f, CColour colour = {255, 255, 255}, TextAlignment alignment = TEXTALIGN_LEFT );

		const char *TextInput( int iId, Vector3f vPosition );
		bool Slider( int id, Vector3f pos, Vector3f size, int max, int &value ); // Adjustable slider
		bool HorzSlider( int id, Vector3f pos, Vector3f size, int max, int &value ); // Same as Slider but slides horizontally
		bool CheckBox( int id, Vector3f pos, bool &value ); // Top-left aligned

		// Composite Elements
		bool LabelButton( GuiID id, const char* text, Vector3f position, Vector3f size );
		bool LabelButtonCentered( GuiID id, const char* text, Vector3f position, Vector3f size );
		const char *SelectableTextInput( int id, Vector3f pos, Vector3f size );
};