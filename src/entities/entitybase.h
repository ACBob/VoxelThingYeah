#pragma once

#include "utility/vector.h"
#include "utility/assorted.h"

#include "world/world.h"

class EntityBase
{
	public:
		EntityBase(); // stub
		~EntityBase();

		// Called right after the entity is added to the world
		// Position and rotation are handled already
		virtual void Spawn() = 0;
		
		// Called every tick (usually 1/20th of a second)
		virtual void Tick() = 0;

#ifdef SERVEREXE
		// Called every tick (usually 1/20th of a second)
		// Gets handed the delta time to help work out physics-related stuff
		// Also gets given the world for collision reasons
		void PhysicsTick(float delta, World *world);
#endif

#ifdef CLIENTEXE
		// Update any client-side things we may have, called every frame rather than per tick
		// Uses the client-side version of the world for testing things (i.e player hand)
		void UpdateClient(World *clientSideWorld);
#endif

		// Gets a forward vector based on the rotation
		Vector GetForward()
		{
			return Vector(0, 0, -1).Rotate(rotation).Normal();
		}

		// Updates the collision box to the new position
		void UpdateCollision()
		{
			collisionBox.pos = position;
		}

		Vector position;
		Vector rotation;
		Vector velocity;

		AABB collisionBox;
};