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
#ifdef CLIENTEXE
			mdl = GetCubeModel();
#endif
		};
		void Kill()
		{
			BaseClass::Kill();
		}

#ifdef CLIENTEXE
		void UpdateClient(World *clientSideWorld);

		void Render()
		{
			BaseClass::Render();
		}
#endif

#ifdef SERVEREXE
		void PhysicsTick(float delta, World *world);

		// Not a pointer in the server case
		InputManager inputMan;
#endif

#ifdef CLIENTEXE
		InputManager *inputMan;
		VoxRaycast hand;
		PointedThing pointed;

		// Block type we're gonna place
		blocktype_t selectedBlockType = blocktype_t::DIRT;

		Camera camera;
#endif

		bool onFloor;
};