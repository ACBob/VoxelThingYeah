#include "entities.hpp"

#include "world/world.hpp"
#include "world/raycast.hpp"

#define STEP_SIZE 0.55f

void CPhysicalEntity::update(float dt)
{
    CEntity::update(dt);

    if (m_parent != nullptr)
        return;

    if (!m_collision)
        m_position += m_velocity * dt;
    else
    {
        Vector3f newPosition = m_position + m_velocity * dt;
        CBoundingBox bbox = getBoundingBox();

        bbox.m_vPosition.y = newPosition.y;
        if (m_world->testCollision(bbox))
        {
            m_velocity.y /= 2.0f;

            m_onGround = true;
            m_floorType = bbox.m_nLastTouched;
        }
        else
            m_position.y = newPosition.y;

        // TODO: handle slopes/stairs

        bbox.m_vPosition.x = newPosition.x;
        if (m_world->testCollision(bbox))
        {
            m_velocity.x /= 2.0f;
        }
        else
            m_position.x = newPosition.x;

        bbox.m_vPosition.z = newPosition.z;
        if (m_world->testCollision(bbox))
        {
            m_velocity.z /= 2.0f;
        }
        else
            m_position.z = newPosition.z;
    }

    if (m_gravity)
    {
        // TODO: gravity on world
        m_velocity.y += -9.8f * dt;
    }
    
    Vector3f friction;
    if (!m_onGround)
        friction = m_velocity * 0.7f * -1.0f;
    else
        friction = m_velocity * 0.1f * -1.0f;
    
    m_velocity += friction * dt;
}

void CActorEntity::update(float dt)
{
    m_rotation.y = m_lookRotation.y;

    Vector3f forward = getForward();
    Vector3f right = forward.RotateAxis(1, -90 * DEG2RAD);
    right.y = 0.0f;
    right = right.Normal();

    float speed = 0.40f;

    if (m_inForward)
        m_velocity += forward * speed;
    if (m_inBackward)
        m_velocity += forward * -speed;
    if (m_inLeft)
        m_velocity += right * -speed;
    if (m_inRight)
        m_velocity += right * speed;
    if (m_inUp)
        m_velocity += Vector3f(0.0f, 1.0f, 0.0f) * speed;
    if (m_inDown)
        m_velocity += Vector3f(0.0f, 1.0f, 0.0f) * -speed;


    CPhysicalEntity::update(dt);
}