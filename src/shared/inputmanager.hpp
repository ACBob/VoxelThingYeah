#include "utility/vector.hpp"

#pragma once

#define IN_NO_MOUSE 0
#define IN_LEFT_MOUSE 1 << 0
#define IN_MIDDLE_MOUSE 1 << 1
#define IN_RIGHT_MOUSE 1 << 2
#define IN_WHEEL_UP 1 << 3
#define IN_WHEEL_DOWN 1 << 4

enum INKEY
{
	INKEY_NOTHING = -1,

	INKEY_FRONT = 0,
	INKEY_RIGHT = 1,
	INKEY_BACK = 2,
	INKEY_LEFT = 3,

	INKEY_CHAT = 4,
	INKEY_OUT  = 5, // Escape

	INKEY_LAST // used only to get the possible inkeys
};

enum KBDKEY // Keys unrepresentable by a char
{
	KBD_ESCAPE = 0,
	KBD_SHIFT  = 1,
	KBD_BACKSPACE = 2,
	KBD_RETURN = 3,
};

class InputManager
{
	public:
		InputManager();
		~InputManager();

#ifdef CLIENTEXE
		// A -> Z (char) Is mapped directly to the keycode (so 65 = A on the keyboard)
		// For other keys (Shift, Escape, etc.) use KBD_*
		bool keyboardState[255];
		bool oldKeyboardState[255]; // See above, previous frame

		// Mousepos is the current position of the mouse
		// Mousemovement is the movement of the mouse since last frame
		// Z is ignored
		Vector mousePos, mouseMovement;

		int mouseState = 0;
		int oldMouseState = 0; // stores mouseState from the last frame

		bool inGui = false; // If we're in a GUI menu (i.e pause, chat)

		void Update();
#endif

		bool inputState[INKEY_LAST];
		bool oldInputState[INKEY_LAST];

		template <typename S>
		void serialize(S& s)
		{
			s & inputState & oldInputState;
		};
};