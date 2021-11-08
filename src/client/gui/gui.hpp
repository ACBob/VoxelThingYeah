
#include "network/client.hpp"
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

// OOP POO
class CGui
{
  private:
	struct Vertex
	{
		float x, y, z; //! Z always 0!!
		float u, v;
		// CColour
		float r, g, b;
	};

	struct _Image
	{
		CTexture *m_pTex = nullptr;
		std::vector<Vertex> m_vertices;
	};

	std::vector<Vertex> GetQuad( Vector3f vPosition, Vector3f vSize, CColour colour, Vector3f vStart = Vector3f( 0, 0 ),
								 Vector3f vEnd = Vector3f( 1, 1 ) );
	std::vector<Vertex> GetCharQuad( const int c, Vector3f vPosition, Vector3f vSize, CColour colour );

	unsigned int m_iVbo, m_iVao;
	std::vector<Vertex> m_textVertiecs;
	std::vector<_Image> m_images;

	// Teh epic textTex
	CTexture *m_pTextTex   = nullptr;
	CShader *m_pTextShader = nullptr;

	CTexture *m_pButtonTex	  = nullptr;
	CTexture *m_pTextInpTex	  = nullptr;
	CTexture *m_pSliderTex	  = nullptr;
	CTexture *m_pThumbTex	  = nullptr;
	CTexture *m_pCheckedTex	  = nullptr;
	CTexture *m_pUncheckedTex = nullptr;
	
	std::map<int, std::string> m_textBuffers;

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
	Vector3f m_vMousePos;

	// Used to get the input
	CInputManager *m_pInputMan = nullptr;

	CNetworkClient *m_pClient = nullptr;

	// Update
	void Update();

	void Resize( int x, int y );

	Vector3f m_vScreenCentre;
	Vector3f m_vScreenDimensions;

	// Shared Resources
	// Ideally stuff that would be used in multiple places
	CTexture *m_pBGTex		  = nullptr;
	CTexture *m_pCrosshairTex = nullptr;
	CTexture *m_pInventoryTex = nullptr;
	CTexture *m_pLogoTex	  = nullptr;
	CTexture *m_pPackPNG = nullptr;

	// Current button pressed down
	int m_iMouseState;
	// Item under the mouse
	int m_iHotItem;
	// Current interaction
	int m_iActiveItem;
	// Current element with keyboard element
	int m_iKeyboardItem;

	int64_t m_iTick = 0;

	// Gui Size
	int m_iGuiUnit;

	// Clears all the stored buffer stuff
	void ClearBuffers();

	void SetTextBuffer( int id, const char *text );
	const char *GetTextBuffer( int id );

	// Z Ignored!
	bool RegionHit( Vector3f vPosition, Vector3f vSize );

	// Converts positions to screen ones, including negatives being from the opposite side and stuff
	Vector3f GetInScreen( Vector3f vPosition );

	enum TextAlignment {
		TEXTALIGN_LEFT	 = 0x0,	 // |LEFT    |
		TEXTALIGN_CENTER = 0x01, // | CENTER |
		TEXTALIGN_RIGHT	 = 0x02	 // |   RIGHT|
	};

	// Utility
	int GetTextLength( const char *text );

	// Elements
	int Button( int iId, Vector3f vPosition, Vector3f vSize, Vector3f vOrigin = Vector3f( 0, 0 ), CTexture *tex = nullptr,
				bool hide = false );
	int AtlasButton( int id, CTexture *tex, Atlas atlas, float atlasDivisions, Vector3f pos, Vector3f size,
					 Vector3f vOrigin = Vector3f( 0, 0 ) );
	int LabelButton( int id, const char *msg, Vector3f pos, Vector3f vOrigin = Vector3f( 0, 0 ),
					 Vector3f padding = Vector3f( 2, 1, 0 ), Vector3f minsize = Vector3f( 0, 0 ) );
	void Label( const char *cText, Vector3f vPosition, CColour colour = CColour( 255, 255, 255 ),
				TextAlignment textAlign = TEXTALIGN_LEFT );
	void Image( CTexture *pTex, Vector3f vPosition, Vector3f vSize, Vector3f vOrigin = Vector3f( 0, 0 ),
				CColour tint = CColour( 255, 255, 255 ) );
	void ImageAtlas( CTexture *pTex, Atlas atlas, float fAtlasDivisions, Vector3f vPosition, Vector3f vSize,
					 Vector3f vOrigin = Vector3f( 0, 0 ), CColour tint = CColour( 255, 255, 255 ) );
	void Image9Rect( CTexture *pTex, Vector3f pos, Vector3f size, CColour color );
	void Crosshair(); // A wrapper for Image
	const char *TextInput( int iId, Vector3f vPosition );
	const char *SelectableTextInput( int id, Vector3f pos, Vector3f size,
									 CTexture *pTex = nullptr ); // Variant of TextInput that has support for selection,
																 // also rendering with a background
	bool Slider( int id, Vector3f pos, Vector3f size, int max, int &value );
	bool HorzSlider( int id, Vector3f pos, Vector3f size, int max, int &value ); // Same as Slider but horizontal
	bool CheckBox( int id, Vector3f pos, Vector3f size, bool &value );
};