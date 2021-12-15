#include "shtoigui.hpp"

#include <glad/glad.h>

#include <algorithm>

CShtoiGUI::CShtoiGUI( ShtoiGUI_displayMode displayMode, float virtualScreenSizeX, float virtualScreenSizeY )
{
    m_displayMode = displayMode;
    m_fVirtualScreenSizeX = virtualScreenSizeX;
    m_fVirtualScreenSizeY = virtualScreenSizeY;

    m_nActiveElement = 0;
    m_nHoverElement = 0;
    m_nPressedElement = 0;
    m_nKeyboardFocusElement = 0;

    m_layoutType = ShtoiGUI_layoutType::Normal;
    m_fLayoutX = 0.0f;
    m_fLayoutY = 0.0f;
    m_fLayoutSizeX = 0.0f;
    m_fLayoutSizeY = 0.0f;
    m_fLayoutSpacingX = 0.0f;
    m_fLayoutSpacingY = 0.0f;

    m_fVirtualCursorX = 0.0f;
    m_fVirtualCursorY = 0.0f;

    // Rendering
    glGenVertexArrays( 1, &m_nVAO );
    glGenBuffers( 1, &m_nVBO );

    glBindVertexArray( m_nVAO );
    glBindBuffer( GL_ARRAY_BUFFER, m_nVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(Vertex) * 4, NULL, GL_DYNAMIC_DRAW );
    
    // Position
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0 );

    // UV
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u) );

    // Color
    glEnableVertexAttribArray( 2 );
    glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r) );

    glBindVertexArray( 0 );
}

CShtoiGUI::~CShtoiGUI()
{
    glDeleteVertexArrays( 1, &m_nVAO );
    glDeleteBuffers( 1, &m_nVBO );
}

void CShtoiGUI::_Quad( float x, float y, float z, float w, float h, float u, float v, float u1, float v1, float r, float g, float b, float a )
{
    Vertex vertices[6] = {
        { x, y, z, u, v, r, g, b, a },
        { x + w, y, z, u1, v, r, g, b, a },
        { x + w, y + h, z, u1, v1, r, g, b, a },
        { x, y, z, u, v, r, g, b, a },
        { x + w, y + h, z, u1, v1, r, g, b, a },
        { x, y + h, z, u, v1, r, g, b, a }
    };

    m_vertices.insert( m_vertices.end(), vertices, vertices + 6 );
}

void CShtoiGUI::Update()
{
    m_nActiveElement = 0;

    // render
    // Sort vertices by the z coordinate
    std::sort( m_vertices.begin(), m_vertices.end(), []( const Vertex &a, const Vertex &b ) {
        return a.z < b.z;
    } );
    // now set all z to 0
    for ( Vertex &v : m_vertices )
    {
        v.z = 0.0f;
    }

    // Disable depth test so that the quad is rendered on top of everything else
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_CULL_FACE );

    glBindVertexArray( m_nVAO );
    glBindBuffer( GL_ARRAY_BUFFER, m_nVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), &m_vertices[0], GL_DYNAMIC_DRAW );

    glDrawArrays( GL_TRIANGLES, 0, m_vertices.size() );

    glBindVertexArray( 0 );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    m_vertices.clear();
}


void CShtoiGUI::BeginLayout( ShtoiGUI_layoutType layoutType, float x, float y, float z, float sizeX, float sizeY, float spacingX, float spacingY )
{
    m_layoutType = layoutType;
    m_fLayoutX = x;
    m_fLayoutY = y;
    m_fLayoutSizeX = sizeX;
    m_fLayoutSizeY = sizeY;
    m_fLayoutSpacingX = spacingX;
    m_fLayoutSpacingY = spacingY;
}

void CShtoiGUI::EndLayout()
{
    m_layoutType = ShtoiGUI_layoutType::Normal;
    m_fLayoutX = 0.0f;
    m_fLayoutY = 0.0f;
    m_fLayoutSizeX = 0.0f;
    m_fLayoutSizeY = 0.0f;
    m_fLayoutSpacingX = 0.0f;
    m_fLayoutSpacingY = 0.0f;
}

void CShtoiGUI::Rect( float x, float y, float z, float w, float h, float r, float g, float b, float a )
{
    if( m_layoutType == ShtoiGUI_layoutType::Normal )
    {
        _Quad( x, y, z, w, h, 0.0f, 0.0f, 1.0f, 1.0f, r, g, b, a );
    }
    else if( m_layoutType == ShtoiGUI_layoutType::Horizontal )
    {
        _Quad( m_fLayoutX, m_fLayoutY, z, w, m_fLayoutSizeY, 0.0f, 0.0f, 1.0f, 1.0f, r, g, b, a );
        m_fLayoutX += w + m_fLayoutSpacingX;
    }
    else if( m_layoutType == ShtoiGUI_layoutType::Vertical )
    {
        _Quad( m_fLayoutX, m_fLayoutY, z, m_fLayoutSizeX, h, 0.0f, 0.0f, 1.0f, 1.0f, r, g, b, a );
        m_fLayoutY += h + m_fLayoutSpacingY;
    }
}