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

        m_onGround = false;

        bbox.m_vPosition.y = newPosition.y;
        if (!m_world->testCollision(bbox))
            m_position.y = newPosition.y;
        else {
            m_velocity.y = 0;
            bbox.m_vPosition.y = m_position.y;

            m_onGround = true;
            m_floorType = bbox.m_nLastTouched;
        }

        bbox.m_vPosition.x = newPosition.x;
        if (!m_world->testCollision(bbox))
            m_position.x = newPosition.x;
        else {
            m_velocity.x = 0;
            bbox.m_vPosition.x = m_position.x;
        }

        bbox.m_vPosition.z = newPosition.z;
        if (!m_world->testCollision(bbox))
            m_position.z = newPosition.z;
        else {
            m_velocity.z = 0;
            bbox.m_vPosition.z = m_position.z;
        }
    }

    if (m_gravity)
    {
        // TODO: gravity on world
        m_velocity.y += -9.8f * dt;
    }
    
    Vector3f friction;
    if (!m_onGround)
        friction = m_velocity * 1.5f * -1.0f;
    else
        friction = m_velocity * 2.0f * -1.0f;
    
    m_velocity += friction * dt;

    // clip velocity
    float ySpeed = m_velocity.y;
    if (m_maxSpeed != 0) {
        if (m_velocity.Magnitude() > m_maxSpeed)
            m_velocity.Normal() *= m_maxSpeed;
    }
    if (m_terminalVelocity != 0) {
        m_velocity.y = fmaxf(ySpeed, -m_terminalVelocity);
    }
}

void CActorEntity::update(float dt)
{
    CPhysicalEntity::update(dt);
    m_rotation.y = m_lookRotation.y;

    m_gravity = !m_flying;

    Vector3f forward = getForward();
    Vector3f right = forward.Cross(Vector3f(0, 1, 0));
    right.y = 0.0f;
    right = right.Normal();

    float speed = m_maxSpeed;

    Vector3f movement;

    // Flying movement
    if (m_flying)
    {
        if (m_inForward)
            movement += forward;
        if (m_inBackward)
            movement -= forward;
        if (m_inLeft)
            movement -= right;
        if (m_inRight)
            movement += right;

        if (m_inUp)
            movement += Vector3f(0, 1, 0);
        if (m_inDown)
            movement += Vector3f(0, 1, 0);
    }
    // Ground movement
    else
    {
        if (m_inForward)
            movement += forward;
        if (m_inBackward)
            movement -= forward;
        if (m_inLeft)
            movement -= right;
        if (m_inRight)
            movement += right;

        if (m_inUp && m_onGround)
            m_velocity.y = speed;
    }

    movement = movement.Normal();
    movement *= speed;

    m_velocity += movement * dt;
}