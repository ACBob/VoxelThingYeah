#include "utility/vector.h"
#include "utility/raycast.h"
#include "world/world.h"
#include "world/block.h"


#include "world/physics.h"

#include "camera.h"

#ifdef CLIENTEXE
#include "inputmanager.h"
#include "sound/soundmanager.h"
#endif

#pragma once

// TODO: generic entity class

class Player
{
	public:
		Player();

#ifdef CLIENTEXE
		void Update(World *chunkMan, SoundManager *soundMan);
#elif SERVEREXE
		void Update(World *chunkMan);
#endif
		void Physics(double delta, World *chunkMan);

		void UpdateCollision();

#ifdef CLIENTEXE
		void Input(bool e);
		void MouseInput(float xrel, float yrel);
#endif

		Vector pos;
		Vector velocity;

		AABB collision;

		Camera camera;

		float yaw;
		float pitch;

		bool onFloor = false;

		bool noclipMode = false;

		// Gets rotated to our pitch/yaw
		Vector forward;

#ifdef CLIENTEXE
		// Pointer to Input manager
		InputManager *inputMan;
#endif

		VoxRaycast hand;
		PointedThing pointed;
		// Block type we're gonna place
		blocktype_t selectedBlockType = blocktype_t::DIRT;
};