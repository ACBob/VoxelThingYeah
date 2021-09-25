
#include "network/client.hpp"
#include "rendering/shadermanager.hpp"
#include "rendering/texturemanager.hpp"
#include "shared/inputmanager.hpp"
#include "shared/statemanager.hpp"
#include "utility/vector.hpp"

#include <map>
#include <vector>

#pragma once

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
		CTexture *m_pTex = nullptr;
		std::vector<Vertex> m_vertices;
	};

	std::vector<Vertex> GetQuad( CVector vPosition, CVector vSize, Colour colour, CVector vStart = CVector( 0, 0 ),
								 CVector vEnd = CVector( 1, 1 ) );
	std::vector<Vertex> GetCharQuad( const char *cChar, CVector vPosition, CVector vSize, Colour colour );

	unsigned int m_iVbo, m_iVao;
	std::vector<Vertex> m_textVertiecs;
	std::vector<_Image> m_images;

	// Teh epic textTex
	CTexture *m_pTextTex   = nullptr;
	CShader *m_pTextShader = nullptr;

	CTexture *m_pButtonTex = nullptr;

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
	CVector m_vMousePos;

	// Used to get the input
	CInputManager *m_pInputMan = nullptr;

	CNetworkClient *m_pClient = nullptr;

	// Update
	void Update();

	void Resize( int x, int y );

	CVector m_vScreenCentre;
	CVector m_vScreenDimensions;

	// Shared Resources
	// Ideally stuff that would be used in multiple places
	CTexture *m_pBGTex		  = nullptr;
	CTexture *m_pCrosshairTex = nullptr;
	CTexture *m_pInventoryTex = nullptr;
	CTexture *m_pLogoTex = nullptr;

	// Current button pressed down
	int m_iMouseState;
	// Item under the mouse
	int m_iHotItem;
	// Current interaction
	int m_iActiveItem;

	// Gui Size
	int m_iGuiUnit;

	// Z Ignored!
	bool RegionHit( CVector vPosition, CVector vSize );

	// Converts positions to screen ones, including negatives being from the opposite side and stuff
	CVector GetInScreen( CVector vPosition );

	enum TextAlignment {
		TEXTALIGN_LEFT	 = 0x0,	 // |LEFT    |
		TEXTALIGN_CENTER = 0x01, // | CENTER |
		TEXTALIGN_RIGHT	 = 0x02	 // |   RIGHT|
	};

	// Utility
	int GetTextLength( const char *text );

	// Elements
	int Button( int iId, CVector vPosition, CVector vSize, CTexture *tex = nullptr );
	int AtlasButton( int id, CTexture *tex, Atlas atlas, float atlasDivisions, CVector pos, CVector size );
	int LabelButton( int id, const char *msg, CVector pos, CVector vOrigin = CVector( 0, 0 ),
					 CVector padding = CVector( 2, 1, 0 ) );
	void Label( const char *cText, CVector vPosition, Colour colour = Color( 1, 1, 1 ),
				TextAlignment textAlign = TEXTALIGN_LEFT );
	void Image( CTexture *pTex, CVector vPosition, CVector vSize, CVector vOrigin = CVector( 0, 0 ),
				Colour tint = Colour( 1, 1, 1 ) );
	void ImageAtlas( CTexture *pTex, Atlas atlas, float fAtlasDivisions, CVector vPosition, CVector vSize,
					 CVector vOrigin = CVector( 0, 0 ) );
	void Crosshair(); // A wrapper for Image
	const char *TextInput( int iId, CVector vPosition );
};