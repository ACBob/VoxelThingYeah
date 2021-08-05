#include "utility/vector.h"

#pragma once

#define IN_NO_MOUSE 0
#define IN_LEFT_MOUSE 1 << 0
#define IN_MIDDLE_MOUSE 1 << 1
#define IN_RIGHT_MOUSE 1 << 2

class InputManager
{
	public:
		InputManager();
		~InputManager();

		// A -> Z (char) Is mapped directly to the keycode (so 65 = A on the keyboard)
		bool keyboardState[255];

		// Mousepos is the current position of the mouse
		// Mousemovement is the movement of the mouse since last frame
		// Z is ignored
		Vector mousePos, mouseMovement;

		int mouseState = 0;

};