#pragma once

#include "utility/vector.hpp"
#include "utility/assorted.hpp"

#include "world/world.hpp"

#ifdef CLIENTEXE
#include "rendering/model.hpp"
#include "rendering/shadermanager.hpp"
#endif

#include <string>


class EntityBase
{
	public:
		EntityBase(); // stub
		~EntityBase();

		// Called right after the entity is added to the world
		// Position and rotation are handled already
		virtual void Spawn() = 0;

		// Called when told to remove from the world
		virtual void Kill() = 0;
		
		// Called every tick (usually 1/20th of a second)
		virtual void Tick()
		{
#ifdef CLIENTEXE
			mdl.pos = position;
#endif
		};

		// Called every tick (usually 1/20th of a second)
		// Gets handed the delta time to help work out physics-related stuff
		// Also gets given the world for collision reasons
		virtual void PhysicsTick(float delta, World *world) = 0;

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

		std::string name;

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

		// if isKilled, is removed from the entity list and not processed otherwise
		bool isKilled = false;
};