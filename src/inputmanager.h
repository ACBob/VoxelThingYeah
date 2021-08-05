#include "utility/vector.h"

#pragma once

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

};