// ShtoiGUI
// Little C++ ImGui

#include <vector>

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
    Grid, // Each successive element is placed in a grid, where their x/y coords determine their position (in 3D this also uses the Z coordinate)
    Form, // Each elements are laid out in rows with two elements each
};

class CShtoiGUI {
    protected:
        struct Vertex {
            float x, y, z;
            float u, v;
            float r, g, b, a;
        };

        std::vector<Vertex> m_vertices;

        ShtoiGUI_elementID m_nActiveElement; // The element that is currently being interacted with
        ShtoiGUI_elementID m_nHoverElement; // The element that is currently being hovered over
        ShtoiGUI_elementID m_nPressedElement; // The element that is currently being pressed
        ShtoiGUI_elementID m_nKeyboardFocusElement; // The element that is currently being focused with the keyboard

        ShtoiGUI_displayMode m_displayMode;

        ShtoiGUI_layoutType m_layoutType;

        float m_fLayoutX, m_fLayoutY; // The position of the next element to be added
        float m_fLayoutSizeX, m_fLayoutSizeY; // The size of the next element to be added
        float m_fLayoutSpacingX, m_fLayoutSpacingY; // The spacing between elements in the layout
        
        float m_fVirtualScreenSizeX, m_fVirtualScreenSizeY; // The size of the virtual screen, used to scale-up the GUI and for 3D
        float m_fVirtualCursorX, m_fVirtualCursorY; // The position of the virtual cursor, used to scale-up the GUI and for 3D

        // Rendering
        void _Quad(float x, float y, float z, float w, float h, float u, float v, float u1, float v1, float r, float g, float b, float a);

        unsigned int m_nVAO, m_nVBO;

    public:
        CShtoiGUI( ShtoiGUI_displayMode displayMode, float virtualScreenSizeX, float virtualScreenSizeY );
        ~CShtoiGUI();

        void SetDisplayMode( ShtoiGUI_displayMode displayMode );
        void SetVirtualScreenSize( float virtualScreenSizeX, float virtualScreenSizeY );
        void SetVirtualCursorPosition( float virtualCursorX, float virtualCursorY );

        // Updates & Renders to the screen
        void Update();

        // Layout
        void BeginLayout( ShtoiGUI_layoutType layoutType, float layoutX, float layoutY, float layoutZ, float layoutSizeX, float layoutSizeY, float layoutSpacingX = 0.0f, float layoutSpacingY = 0.0f );
        void EndLayout();

        // Elements
        void Rect( float x, float y, float z, float sizeX, float sizeY, float r, float g, float b, float a );

};
