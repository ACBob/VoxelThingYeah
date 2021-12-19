// ShtoiGUI
// Little C++ ImGui

#include <vector>
#include <wchar.h>

#include "render/material.hpp"

using ShtoiGUI_elementID = unsigned short; // 65535 elements max

/*
Display Modes:
 2D:
  Draws 2D elements that are plastered on the screen.
  Z-order is determined by the Z coordinate.
 3D: (TODO)
  Draws 3D elements that are plastered in-world.
  Raycasts are used to find position of the virtual cursor.
  Z-Coordinate is depth!
*/

enum class ShtoiGUI_displayMode {
    TwoDee,
    ThreeDee
};

enum class ShtoiGUI_layoutType {
    Normal, // Position is hand-entered
    Vertical, // Each successive element is placed below the previous
    Horizontal, // Each successive element is placed to the right of the previous
    Form, // Each elements are laid out in rows with two elements each
    Flex, // Each element tries to take up a percentage of the space. Requires you to use the non-generic BeginFlex()
    FlexRows, // Each element tries to take up a percentage of vertical space.
    FlexColumns, // Each element tries to take up a percentage of horizontal space.
};

enum class ShtoiGUI_buttonState {
    Normal, // Nothing
    Hover, // Mouse is over it
    Pressed, // Mouse JUST Pressed it
    Held, // The mouse is held down on it
};

enum class ShtoiGUI_mouseState : int {
    Left = 1 << 0,
    Right = 1 << 1,
    Middle = 1 << 2,
};

class CShtoiGUI {
    protected:
        struct Vertex {
            float x, y, z;
            float u, v;
            float r, g, b, a;
        };

        struct Quad {
            Vertex vertices[6];
            float z;
            materialSystem::CTexture *m_pTexture;
        };

        std::vector<Quad> m_quads;

        ShtoiGUI_elementID m_nActiveElement; // The element that is currently being interacted with
        ShtoiGUI_elementID m_nHoverElement; // The element that is currently being hovered over
        ShtoiGUI_elementID m_nPressedElement; // The element that is currently being pressed
        ShtoiGUI_elementID m_nKeyboardFocusElement; // The element that is currently being focused with the keyboard

        ShtoiGUI_displayMode m_displayMode;

        struct ShtoiGUI_layout {
            ShtoiGUI_layoutType type; // The type of the layout, controls behaviour
            float posX, posY, posZ; // The position of the layout origin X/Y
            float sizeX, sizeY; // Size of the layout
            float offsetX, offsetY; // The position of the next widget, relative to the layout pos
            float spacingX, spacingY; // The amount of space between widgets

            // extra information used in some layouts
            int extraNumber; // For flex layouts, this is the number of elements
            int extraNumber1; // For flex layouts, this is the bias (usually 1)
        };
        std::vector<ShtoiGUI_layout> m_layoutStack; // Holds the layout on a stack, allowing nested layouts.
        
        float m_fVirtualScreenSizeX, m_fVirtualScreenSizeY; // The size of the virtual screen, used to scale-up the GUI and for 3D
        float m_fVirtualCursorX, m_fVirtualCursorY; // The position of the virtual cursor, used to scale-up the GUI and for 3D
        int m_nMouseState; // State of the mouse, see enum ShtoiGUI_mouseState

        // Layout
        inline void _transformToLayout(float &x, float &y, float &z, float &sizeX, float &sizeY);

        // Rendering
        void _quad(float x, float y, float z, float w, float h, float u, float v, float u1, float v1, float r, float g, float b, float a);

        // Logic
        ShtoiGUI_buttonState _buttonLogic(int id, float x, float y, float w, float h);

        unsigned int m_nVAO, m_nVBO;

        materialSystem::CTexture *m_pTexture;

        struct Character {
            int code = 0x0; // code
            int texNumber = 0; // Where in the texture it is (0-255)
            float width = 1.0f; // size of the char
            materialSystem::CTexture *m_pTexture = nullptr;
        };

        std::map<int, Character> m_charMap;

        void _charQuad(Character character, float x, float y, float z, float w, float h, float r, float g, float b, float a);
        

    public:
        CShtoiGUI( ShtoiGUI_displayMode displayMode, float virtualScreenSizeX, float virtualScreenSizeY );
        ~CShtoiGUI();

        void SetDisplayMode( ShtoiGUI_displayMode displayMode );
        void SetVirtualScreenSize( float virtualScreenSizeX, float virtualScreenSizeY );
        void SetVirtualCursorPosition( float virtualCursorX, float virtualCursorY );

        // Updates & Renders to the screen
        void Update(float mouseX, float mouseY, int mouseState);

        // Layout
        void BeginLayout( ShtoiGUI_layoutType layoutType, float layoutX, float layoutY, float layoutZ, float layoutSizeX, float layoutSizeY, float layoutSpacingX = 0.0f, float layoutSpacingY = 0.0f, int a = 0, int b = 1 );
        void SetLayoutNumbers( int a, int b );
        void EndLayout();

        // Elements
        void Rect( float x, float y, float z, float sizeX, float sizeY, float r, float g, float b, float a );
        void Label( std::string text, float x, float y, float z, float size );

        ShtoiGUI_buttonState Button( int id, float x, float y, float z, float w, float h );
};
