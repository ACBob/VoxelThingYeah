#include "vector.h"

#pragma once

class InputManager
{
	public:
		InputManager();
		~InputManager();

		bool keyboardState[255] = {false};

		// Mousepos is the current position of the mouse
		// Mousemovement is the movement of the mouse since last frame
		// Z is ignored
		Vector mousePos, mouseMovement;

};