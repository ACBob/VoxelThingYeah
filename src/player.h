#include "utility/vector.h"
#include "inputmanager.h"
#include "utility/raycast.h"
#include "world/chunkmanager.h"
#include "world/block.h"

#pragma once

class Player
{
	public:
		Player();

		void Update(ChunkManager *chunkMan);

		void Input(bool e);
		void MouseInput(float xrel, float yrel);

		Vector pos;

		float yaw;
		float pitch;

		// Gets rotated to our pitch/yaw
		Vector forward;

		// Pointer to Input manager
		InputManager *inputMan;

		VoxRaycast hand;
		PointedThing pointed;
		// Block type we're gonna place
		blocktype_t selectedBlockType = blocktype_t::DIRT;
};