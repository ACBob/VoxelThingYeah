#include "entitybase.h"
#include "utility/raycast.h"
#include "inputmanager.h"
#include "camera.h"

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
			this->mdl = GetCubeModel();
#endif
		};

#ifdef CLIENTEXE
		void UpdateClient(World *clientSideWorld);

		void Render()
		{
			BaseClass::Render();
		}
#endif

#ifdef SERVEREXE
		void PhysicsTick(float delta, World *world);
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