#include "utility/vector.h"
#include "inputmanager.h"
#include "utility/raycast.h"
#include "world/chunkmanager.h"
#include "world/block.h"

#include "sound/soundmanager.h"

#include "camera.h"

#pragma once

// TODO: generic entity class

class Player
{
	public:
		Player();

		void Update(ChunkManager *chunkMan, SoundManager *soundMan, double delta);

		void Input(bool e);
		void MouseInput(float xrel, float yrel);

		Vector pos;
		Vector velocity;

		Camera camera;

		float yaw;
		float pitch;

		bool onFloor = false;

		// Gets rotated to our pitch/yaw
		Vector forward;

		// Pointer to Input manager
		InputManager *inputMan;

		VoxRaycast hand;
		PointedThing pointed;
		// Block type we're gonna place
		blocktype_t selectedBlockType = blocktype_t::DIRT;
};