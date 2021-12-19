// -- Entity Player
// The player entity.

#include "entityphysics.hpp"

class CEntityPlayer : public CBasePhysicsEntity
{
    DeclBaseClass(CBasePhysicsEntity);

    public:
    CEntityPlayer(entityId_t id);
    ~CEntityPlayer();

    void Update(float dt);
};