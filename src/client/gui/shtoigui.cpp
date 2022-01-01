#include "shtoigui.hpp"

#include <algorithm>

#include "shared/logging.hpp"
#include "utility/toml.hpp"
#include "shared/filesystem.hpp"

#include "render/model.hpp"

#include "utility/utfz.h"

CShtoiGUI::CShtoiGUI( ShtoiGUI_displayMode displayMode, float virtualScreenSizeX, float virtualScreenSizeY )
{
    m_displayMode = displayMode;
    m_fVirtualScreenSizeX = virtualScreenSizeX;
    m_fVirtualScreenSizeY = virtualScreenSizeY;

    m_nActiveElement = 0;
    m_nHoverElement = 0;
    m_nPressedElement = 0;
    m_nKeyboardFocusElement = 0;

    m_fVirtualCursorX = 0.0f;
    m_fVirtualCursorY = 0.0f;
    m_nMouseState = 0;

    m_pModel = new rendering::models::CModel();
    m_pModel->m_pShader = rendering::materials::GetNamedShader( "gui" );

    // Vertices are given individually per quad, but the faces remain constant
    m_pModel->m_faces = {
        { 2, 1, 0 },
        { 3, 2, 0 }
    };

    // now handle the font definition file
    bool bSuccess = false;
    int64_t fileSize = 0;
    char *data = (char*)fileSystem::LoadFile("/assets/lang/font.toml", fileSize, bSuccess);
    toml::table root = toml::parse(data);

    // [font]
    // textures = [""] # array of texture files
    // TODO: glue each texture together and such
    if (!root.contains("font") || !root["font"].is_table()) {
        con_error("Font file does not have [font]");
        return;
    }
    toml::table *fontTble = root["font"].as_table();
    if (!fontTble->contains("textures") || !fontTble->get("textures")->is_array()) {
        con_error("Font file does not have font.textures");
        return;
    }
    if (!fontTble->contains("characters") || !fontTble->get("characters")->is_table()) {
        con_error("Font file does not have a font.characters");
        return;
    }
    toml::table *fontChars = fontTble->get("characters")->as_table();

    // TODO: we pretty heavily rely on the material system giving us already loaded textures
    toml::array *fontTextures = fontTble->get("textures")->as_array();
    // m_pTexture = materialSystem::LoadTexture(fontTextures->get_as<std::string>(0)->value_or("/assets/textures/font.png"));

    m_pTexture = nullptr;

    // Now load the char definitions
    int tex = 0;
    char *texNumBuf = new char[16];
    for ( auto &k : *fontChars )
    {
        // Load texture
        // TODO: enforce size limit
        // TODO: error handling
        sprintf(texNumBuf, "%d", tex);
        toml::array *charsArray = fontChars->get(texNumBuf)->as_array();

        std::string texName = fontTextures->get(tex)->value_or<std::string>("font/cp437.png");
        rendering::materials::CTexture *pTex = rendering::materials::LoadTexture(texName);

        con_debug("%s", texName.c_str());

        int resX = pTex->m_nWidth / 16;
        int resY = pTex->m_nHeight / 16;

        int i = 0;
        for ( toml::node& elem : *charsArray ) {
            Character chr;
            chr.code = elem.as_integer()->value_or(0xfffd); // TODO: handle error
            chr.texNumber = i;
            chr.m_pTexture = pTex;

            int px = ( i % 16 ) * resX;
            int py = ( i / 16 ) * resY;

            chr.width = 1.0f;
            for (int y = 0; y < resY; y++)
            {
                for (int x = 0; x < resX; x++)
                {
                    int idx = 4 * ( py + y ) * pTex->m_nWidth + 4 * ( px + x ) + 3;
                    if ( pTex->m_uchImageData[idx] && x + 1 >= chr.width )
                    {
                        chr.width = x + 1;
                    }
                }
            }
            chr.width /= resX;

            i++;
            m_charMap[chr.code] = chr;
        }

        tex++;
    }

    con_debug("Loaded %d chars", m_charMap.size() );
    con_debug("With %d pages", tex);
}

CShtoiGUI::~CShtoiGUI()
{
    delete m_pModel;
}

void CShtoiGUI::_quad( float x, float y, float z, float w, float h, float u, float v, float u1, float v1, float r, float g, float b, float a )
{
    Quad quad = {
        {
            { x, y, z, 0, 0, -1, u, v, r, g, b },
            { x + w, y, z, 0, 0, -1, u1, v, r, g, b },
            { x + w, y + h, z, 0, 0, -1, u1, v1, r, g, b },
            { x, y + h, z, 0, 0, -1, u, v1, r, g, b }
        },
        z,
        nullptr, // TODO: texture
    };

    m_quads.insert( m_quads.end(), quad );
}

void CShtoiGUI::_charQuad(Character character, float x, float y, float z, float w, float h, float r, float g, float b, float a)
{
    float u = (character.texNumber % 16) / 16.0f;
    float v = (character.texNumber / 16) / 16.0f;
    float u1 = u + 1 / 16.0f;
    float v1 = v + 1 / 16.0f;

    Quad quad = {
        {
            { x, y, z, 0, 0, -1, u, v, r, g, b },
            { x + w, y, z, 0, 0, -1, u1, v, r, g, b },
            { x + w, y + h, z, 0, 0, -1, u1, v1, r, g, b },
            { x, y + h, z, 0, 0, -1, u, v1, r, g, b }
        },
        z,
        character.m_pTexture,
    };
    
    m_quads.insert( m_quads.end(), quad );
}

ShtoiGUI_buttonState CShtoiGUI::_buttonLogic(int id, float x, float y, float w, float h)
{
    ShtoiGUI_buttonState state = ShtoiGUI_buttonState::Normal;

    // test mouse position against bounds
    if ( m_fVirtualCursorX >= x && m_fVirtualCursorY >= y && m_fVirtualCursorX <= x + w && m_fVirtualCursorY <= y + h )
    {
        state = ShtoiGUI_buttonState::Hover;
        m_nHoverElement = id;

        if (m_nMouseState & (int)ShtoiGUI_mouseState::Left)
        {
            state = ShtoiGUI_buttonState::Held;
            m_nActiveElement = id;
        }
    }

    return state;
}

void CShtoiGUI::Update(float mouseX, float mouseY, int mouseState)
{
    m_nActiveElement = 0;
    m_nHoverElement = 0;

    m_nMouseState = mouseState;
    m_fVirtualCursorX = mouseX;
    m_fVirtualCursorY = mouseY;

    // render
    // TODO: Sort vertices by the z coordinate
    std::sort( m_quads.begin(), m_quads.end(), []( const Quad &a, const Quad &b ) {
        return a.z < b.z;
    } );

    // Shouldn't happen, but also don't want to endlessly push layouts...
    if (m_layoutStack.size()) {
        con_warning("Layout not closed!!");
        m_layoutStack.clear();
    }

    // Disable depth test so that the quad is rendered on top of everything else
    // glDisable( GL_DEPTH_TEST );
    // glDisable( GL_CULL_FACE );

    // glBindVertexArray( m_nVAO );
    // glBindBuffer( GL_ARRAY_BUFFER, m_nVBO );;

    for ( const Quad& q : m_quads )
    {
        m_pModel->m_vertices.clear();
        // q.vertices is an array of 6 vertices
        m_pModel->m_vertices.push_back( q.vertices[0] );
        m_pModel->m_vertices.push_back( q.vertices[1] );
        m_pModel->m_vertices.push_back( q.vertices[2] );
        m_pModel->m_vertices.push_back( q.vertices[3] );
        m_pModel->m_vertices.push_back( q.vertices[4] );
        m_pModel->m_vertices.push_back( q.vertices[5] );
        m_pModel->Update();

        if (q.m_pTexture != nullptr)
            m_pModel->m_pTexture = q.m_pTexture;

        m_pModel->Render( Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(1, 1, 1) );
    }

    // glBindVertexArray( 0 );

    // glEnable( GL_DEPTH_TEST );
    // glEnable( GL_CULL_FACE );

    m_quads.clear();
}


void CShtoiGUI::BeginLayout( ShtoiGUI_layoutType layoutType, float x, float y, float z, float sizeX, float sizeY, float spacingX, float spacingY, int a, int b )
{
    _transformToLayout( x, y, z, sizeX, sizeY );

    m_layoutStack.push_back( {
        layoutType,
        x, y, z,
        sizeX, sizeY,
        0, 0,
        spacingX, spacingY,
        a, b,
    } );
}

inline void CShtoiGUI::_transformToLayout(float &x, float &y, float &z, float &sizeX, float &sizeY)
{
    if (m_layoutStack.size() == 0)
        return;

    ShtoiGUI_layout &currentLayout = m_layoutStack.back();

    switch (currentLayout.type)
    {
        default:
        case ShtoiGUI_layoutType::Normal:
            return;
        break;
        case ShtoiGUI_layoutType::Vertical:
            x = currentLayout.posX + currentLayout.offsetX;
            y = currentLayout.posY + currentLayout.offsetY;
            z += currentLayout.posZ; // Our depth is relative to this, it acts like a container

            currentLayout.posY += sizeY;
        break;
        case ShtoiGUI_layoutType::Horizontal:
            x = currentLayout.posX + currentLayout.offsetX;
            y = currentLayout.posY + currentLayout.offsetY;
            z += currentLayout.posZ; // Our depth is relative to this, it acts like a container

            currentLayout.offsetX += sizeX;
        break;
        case ShtoiGUI_layoutType::Form:
            // TODO: Implement form (use the different args as the number to work out row/column)
        break;
        case ShtoiGUI_layoutType::Flex:
            // TODO: act more like a grid, instead of diagonal
            x = currentLayout.posX + currentLayout.offsetX;
            y = currentLayout.posY + currentLayout.offsetY;
            z += currentLayout.posZ;

            sizeX = (currentLayout.sizeX / currentLayout.extraNumber) * currentLayout.extraNumber1;
            sizeY = (currentLayout.sizeY / currentLayout.extraNumber) * currentLayout.extraNumber1;

            currentLayout.offsetX += sizeX;
            currentLayout.offsetY += sizeY;
            currentLayout.extraNumber1 = 1;
        break;
        case ShtoiGUI_layoutType::FlexRows:
            x = currentLayout.posX + currentLayout.offsetX;
            y = currentLayout.posY + currentLayout.offsetY;
            z += currentLayout.posZ;

            sizeX = currentLayout.sizeX;
            sizeY = (currentLayout.sizeY / currentLayout.extraNumber) * currentLayout.extraNumber1;

            currentLayout.offsetY += sizeY;
            currentLayout.extraNumber1 = 1;
        break;
        case ShtoiGUI_layoutType::FlexColumns:
            x = currentLayout.posX + currentLayout.offsetX;
            y = currentLayout.posY + currentLayout.offsetY;
            z += currentLayout.posZ;

            sizeX = (currentLayout.sizeX / currentLayout.extraNumber) * currentLayout.extraNumber1;
            sizeY = currentLayout.sizeY;

            currentLayout.offsetX += sizeX;
            currentLayout.extraNumber1 = 1;
        break;
    }
}

void CShtoiGUI::SetLayoutNumbers(int a, int b)
{
    if (m_layoutStack.size() == 0)
        return;

    ShtoiGUI_layout &layout = m_layoutStack.back();
    layout.extraNumber = a;
    layout.extraNumber1 = b;
}

void CShtoiGUI::EndLayout()
{
    m_layoutStack.pop_back();
}

void CShtoiGUI::Rect( float x, float y, float z, float w, float h, float r, float g, float b, float a )
{
    _transformToLayout(x, y, z, w, h);
    _quad( x, y, z, w, h, 0.0f, 0.0f, 1.0f, 1.0f, r, g, b, a );
}

ShtoiGUI_buttonState CShtoiGUI::Button( int id, float x, float y, float z, float w, float h )
{
    _transformToLayout(x, y, z, w, h);

    ShtoiGUI_buttonState state = _buttonLogic(id, x, y, w, h);

    float r, g, b;
    r = g = b = 1.0f;

    if (state == ShtoiGUI_buttonState::Hover) {
        r = 0.8f;
        g = 0.6f;
        b = 1.0f;
    }
    else if (state == ShtoiGUI_buttonState::Pressed || state == ShtoiGUI_buttonState::Held) {
        r = g = b = 0.5f;
    }

    _quad( x, y, z, w, h, 0.0f, 0.0f, 1.0f, 1.0f, r, g, b, 1.0f);

    return state;
}

void CShtoiGUI::Label( std::string text, float x, float y, float z, float size ) {
    float w, h; // ignored, just here to satisfy transformToLayout
    _transformToLayout(x, y, z, w, h);


    int i = 0;
    int c = 0;
    // int lastKnownChar = 0;

    float letterSpacing = ( 1.0f / 8.0f ) * size;

    const char* str = text.c_str();

    while ( utfz::next(str, c) )
    {
        Character chr;
        if (m_charMap.find(c) == m_charMap.end()) {
            chr.code = 0xfffd; // UNICODE_REPLACEMENT_CHARACTER
            chr.m_pTexture = m_pTexture;
            chr.texNumber = 255;
        }
        else {
            chr = m_charMap.at(c);
        }

        // TODO: formatting codes

        _charQuad( chr, x + i, y, z, size, size, 1, 1, 1, 1 );

        i += chr.width * size;
        i += letterSpacing;
    }
}