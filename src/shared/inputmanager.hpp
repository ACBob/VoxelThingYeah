#include "utility/vector.hpp"

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

	INKEY_LAST // used only to get the possible inkeys
};

enum KBDKEY // Keys unrepresentable by a char
{ KBD_ESCAPE	= 0,
  KBD_SHIFT		= 1,
  KBD_BACKSPACE = 2,
  KBD_RETURN	= 3,
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

	template <typename S> void serialize( S &s ) { s &m_bInputState &m_bOldInputState; };
};