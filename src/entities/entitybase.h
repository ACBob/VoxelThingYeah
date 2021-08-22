#pragma once

#include "utility/vector.h"
#include "utility/assorted.h"

#include "world/world.h"

#ifdef CLIENTEXE
#include "rendering/model.h"
#include "rendering/shadermanager.h"
#endif


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
		virtual void PhysicsTick(float delta, World *world) = 0;
#endif

#ifdef CLIENTEXE
		// Update any client-side things we may have, called every frame rather than per tick
		// Uses the client-side version of the world for testing things (i.e player hand)
		virtual void UpdateClient(World *clientSideWorld) = 0;

		// Renders our Model.
		virtual void Render()
		{
			mdl.Render();
		}

		virtual void SetShader(Shader* shader)
		{
			mdl.SetShader(shader);
		}

		Model mdl;
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


		template <typename S>
		void serialize(S& s)
		{
			s.container4b(position, 3);
			s.container4b(rotation, 3);
			s.container4b(velocity, 3);
			s.class4b(collisionBox);
		};
};