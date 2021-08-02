#include "vector.h"
#include "inputmanager.h"

#pragma once

class Player
{
	public:
		Player();

		void Update();

		void Input(bool e);
		void MouseInput(float xrel, float yrel);

		Vector pos = Vector(0.0f, 0.0f, 5.0f);

		float yaw;
		float pitch;

		// Gets rotated to our pitch/yaw
		Vector forward = Vector(0.0f, 0.0f, 1.0f);

		// Pointer to Input manager
		InputManager *inputMan;
};