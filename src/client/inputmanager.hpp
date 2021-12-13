#include "utility/vector.hpp"

#include <string>

#pragma once

enum INMOUSE {
    IN_NO_MOUSE = 0,
    IN_LEFT_MOUSE = 1 << 0,
    IN_RIGHT_MOUSE = 1 << 1,
    IN_MIDDLE_MOUSE = 1 << 2,
    IN_WHEEL_UP = 1 << 3,
    IN_WHEEL_DOWN = 1 << 4,
};

enum CONTROLS {
	CONTROLS_NOTHING = -1,

	CONTROLS_FRONT = 0,
	CONTROLS_RIGHT = 1,
	CONTROLS_BACK	= 2,
	CONTROLS_LEFT	= 3,

	CONTROLS_UP   = 4,
	CONTROLS_DOWN = 5,

	CONTROLS_CHAT = 6,
	CONTROLS_OUT  = 7, // Escape

	CONTROLS_FLY = 8,

	CONTROLS_INV = 9,

	CONTROLS_LAST // used only to get the possible inkeys, not a key of its own
};

enum KBDKEY // Keys unrepresentable by a char, we have about 32 of these available to us
{ KBD_ESCAPE	= 0,
  KBD_SHIFT		= 1,
  KBD_BACKSPACE = 2,
  KBD_RETURN	= 3,
  KBD_CNTRL		= 4,

  // arrow keys
  KBD_UP	= 5,
  KBD_DOWN	= 6,
  KBD_LEFT	= 7,
  KBD_RIGHT = 8,

  // Other keys
  KBD_PGUP	 = 9,
  KBD_PGDN	 = 10,
  KBD_DELETE = 11,
  KBD_ALT	 = 12,

  KBD_F1  = 13,
  KBD_F2  = 14,
  KBD_F3  = 15,
  KBD_F4  = 16,
  KBD_F5  = 17,
  KBD_F6  = 18,
  KBD_F7  = 19,
  KBD_F8  = 20,
  KBD_F9  = 21,
  KBD_F10 = 22,
  KBD_F11 = 23,
  KBD_F12 = 24,

  KBD_TAB = 25,
};

// The cursor to present to the user
enum CURSORS {
	CURSOR_ARROW = 0, // SDL_SYSTEM_CURSOR_ARROW
	CURSOR_HAND	 = 1, // SDL_SYSTEM_CURSOR_HAND
};

class CInputManager
{
  public:
	CInputManager();
	~CInputManager();

	// A -> Z (char) Is mapped directly to the keycode (so 65 = A on the keyboard)
	// For other keys (Shift, Escape, etc.) use KBD_*
	bool m_bKeyboardState[255];
	bool m_bOldKeyboardState[255]; // See above, previous frame

	// the character we are to type
	char *m_cTypeKey = nullptr;

	std::string m_clipboard;

	// Mousepos is the current position of the mouse
	// Mousemovement is the movement of the mouse since last frame
	// Z is ignored
	Vector3i m_vMousePos, m_vMouseMovement;

	int m_iMouseState	 = 0;
	int m_iOldMouseState = 0; // stores mouseState from the last frame

	// Sets the cursor to the given cursor
	CURSORS m_iCursor = CURSOR_ARROW;

	bool m_bInGui = false; // If we're in a GUI menu (i.e pause, chat)

	void Update();

	bool m_bInputState[CONTROLS_LAST];
	bool m_bOldInputState[CONTROLS_LAST];
};