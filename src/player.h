#include "vector.h"

#pragma once

class Player
{
	public:
		Player();

		void Input(bool e);

		Vector pos = Vector(0.0f, 0.0f, -5.0f);
		Vector dir = Vector(0.0f, 0.0f, 1.0f);
};