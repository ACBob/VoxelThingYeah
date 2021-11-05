#include "utility/vector.hpp"

#include <string>

#pragma once

#define IN_NO_MOUSE 0
#define IN_LEFT_MOUSE 1 << 0
#define IN_MIDDLE_MOUSE 1 << 1
#define IN_RIGHT_MOUSE 1 << 2
#define IN_WHEEL_UP 1 << 3
#define IN_WHEEL_DOWN 1 << 4

enum INKEY {
	INKEY_NOTHING = -1,

	INKEY_FRONT = 0,
	INKEY_RIGHT = 1,
	INKEY_BACK	= 2,
	INKEY_LEFT	= 3,

	INKEY_UP   = 4,
	INKEY_DOWN = 5,

	INKEY_CHAT = 6,
	INKEY_OUT  = 7, // Escape

	INKEY_FLY = 8,

	INKEY_INV = 9,

	INKEY_LAST // used only to get the possible inkeys, not a key of its own
};

enum KBDKEY // Keys unrepresentable by a char, we have about 32 of these available to us
{ KBD_ESCAPE	= 0,
  KBD_SHIFT		= 1,
  KBD_BACKSPACE = 2,
  KBD_RETURN	= 3,
  KBD_CNTRL		= 4,

  // arrow keys
  KBD_UP = 5,
  KBD_DOWN = 6,
  KBD_LEFT = 7,
  KBD_RIGHT = 8,

  // Other keys
  KBD_PGUP = 9,
  KBD_PGDN = 10,
  KBD_DELETE = 11,
  KBD_ALT = 12,

  KBD_F1 = 13,
  KBD_F2 = 14,
  KBD_F3 = 15,
  KBD_F4 = 16,
  KBD_F5 = 17,
  KBD_F6 = 18,
  KBD_F7 = 19,
  KBD_F8 = 20,
  KBD_F9 = 21,
  KBD_F10 = 22,
  KBD_F11 = 23,
  KBD_F12 = 24,

  KBD_TAB = 25,
};

class CInputManager
{
  public:
	CInputManager();
	~CInputManager();

#ifdef CLIENTEXE
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
	CVector m_vMousePos, m_vMouseMovement;

	int m_iMouseState	 = 0;
	int m_iOldMouseState = 0; // stores mouseState from the last frame

	bool m_bInGui = false; // If we're in a GUI menu (i.e pause, chat)

	void Update();
#endif

	bool m_bInputState[INKEY_LAST];
	bool m_bOldInputState[INKEY_LAST];
};