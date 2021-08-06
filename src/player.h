#include "utility/vector.h"
#include "inputmanager.h"

#pragma once

class Player
{
	public:
		Player();

		void Update();

		void Input(bool e);
		void MouseInput(float xrel, float yrel);

		Vector pos;

		float yaw;
		float pitch;

		// Gets rotated to our pitch/yaw
		Vector forward;

		// Pointer to Input manager
		InputManager *inputMan;
};