// -- Entity Physics
// Base class for any and all entities that can be affected by physics.
// This physics is more akin to Quake 2's physics, than actual physics.
// (i.e gravity pulling down, friction slowing down, etc)

#include "entitybase.hpp"
#include "world/physics.hpp"

// Max. height a step can be in blocks
#define STEP_SIZE 0.55f

class CBasePhysicsEntity : virtual public CEntityBase
{
    DeclBaseClass(CBasePhysicsEntity, CEntityBase);

    public:
    CBasePhysicsEntity(entityId_t id);
    virtual ~CBasePhysicsEntity() {};

    void SetVelocity(const Vector3f& velocity);
    void SetVelocity(float x, float y, float z);

    Vector3f GetVelocity();
    void GetVelocity(float& x, float& y, float& z);

    virtual void PhysicsUpdate(float dt);

    protected:
    Vector3f m_velocity;
    
    bool m_bCollides;
    bool m_bGravity;

    bool m_bFloor; // Is on the floor?
    uint16_t m_nFloor; // Block ID of the floor

    Vector3f m_gravity;
    Vector3f m_friction;

    CBoundingBox m_boundingBox = {
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    }; // Used for collision detection
};