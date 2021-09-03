#include "entitybase.hpp"
#include "utility/raycast.hpp"
#include "shared/inputmanager.hpp"
#include "camera.hpp"

#pragma once

class EntityPlayer : public EntityBase
{
	DeclBaseClass(EntityBase);

	public:
		EntityPlayer();
		~EntityPlayer();

		void Tick();
		void Spawn()
		{
			BaseClass::Spawn();
#ifdef CLIENTEXE
			mdl = modelSystem::LoadModel("models/player.obj");
#endif
		};
		void Kill()
		{
			BaseClass::Kill();
		}

#ifdef CLIENTEXE
		void UpdateClient(World *clientSideWorld);
#endif

		void PhysicsTick(float delta, World *world);

#ifdef CLIENTEXE
		InputManager *inputMan = nullptr;
		VoxRaycast hand;
		PointedThing pointed;

		// Block type we're gonna place
		blocktype_t selectedBlockType = blocktype_t::DIRT;

		Camera camera;

		void *client = nullptr;
#endif

		bool onFloor;
};