// -- Entity Player
// The player entity.

#include "entityphysics.hpp"

class CEntityPlayer : virtual public CBasePhysicsEntity
{
    DeclBaseClass(CEntityPlayer, CBasePhysicsEntity);

    public:
    CEntityPlayer(entityId_t id);
    ~CEntityPlayer();

    void Update(float dt);
};