// -- Entity Player
// The player entity.

#include "entityphysics.hpp"

class CEntityPlayer : virtual public CBasePhysicsEntity
{
    DeclBaseClass(CEntityPlayer, CBasePhysicsEntity);

    public:
    CEntityPlayer(entityId_t id, CWorld *pWorld);
    ~CEntityPlayer();

    void Update(float dt);
};