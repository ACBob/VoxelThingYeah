#include "entityphysics.hpp"

#include "world/physics.hpp"
#include "world/world.hpp"
#include "world/raycast.hpp"

CBasePhysicsEntity::CBasePhysicsEntity(entityId_t id) : BaseClass(id),
    m_boundingBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.0f, 1.0f), Vector3f(0.5f, 0.5f, 0.5f))
{
    m_bFloor = false;
    m_nFloor = 0;
    m_bCollides = true;
    m_bGravity = true;
    m_gravity = Vector3f(0.0f, -9.8f, 0.0f);
}

void CBasePhysicsEntity::SetVelocity(const Vector3f& velocity)
{
    m_velocity = velocity;
}
void CBasePhysicsEntity::SetVelocity(float x, float y, float z)
{
    m_velocity.x = x;
    m_velocity.y = y;
    m_velocity.z = z;
}

Vector3f CBasePhysicsEntity::GetVelocity()
{
    return m_velocity;
}
void CBasePhysicsEntity::GetVelocity(float& x, float& y, float& z)
{
    x = m_velocity.x;
    y = m_velocity.y;
    z = m_velocity.z;
}

void CBasePhysicsEntity::PhysicsUpdate(float dt)
{
    if (!m_bCollides)
        m_position += m_velocity * dt;
    else
    {
        Vector3f newPosition = m_position + m_velocity * dt;

        m_boundingBox.m_vPosition.y = newPosition.y;
        if (m_pWorld->testCollision(m_boundingBox))
        {
            m_position.y = m_boundingBox.m_vPosition.y;
            m_velocity.y = 0.0f;

            m_bFloor = true;
            m_nFloor = m_boundingBox.m_nLastTouched;
        }
        else
            m_position.y = newPosition.y;

        // TODO: handle slopes as I think the casting will be jittery and slow 

        m_boundingBox.m_vPosition.x = newPosition.x;
        if (m_pWorld->testCollision(m_boundingBox))
        {
            // Quake seems to try casting a ray to find the step-iness of a collision
            // So create a ray from our current position + the step height facing down
            // We then teleport to that position and test for collision again
            // If we collide, the step is unsuitable (i.e too tall, maybe ceiling too low)
            // And we go back to the original position
            // If we don't collide, the step is suitable (i.e floor is high enough)
            // And we actually teleport there, treating it as if we've just landed on the floor

            Vector3f rayStart = newPosition + Vector3f(0.0f, STEP_SIZE, 0.0f);
            
            std::pair<Vector3f, Vector3f> ray = CRaycast::cast(m_pWorld, rayStart, Vector3f(0.0f, -1.0f, 0.0f), STEP_SIZE);

            // move the bounding box to the new position
            m_boundingBox.m_vPosition = ray.first;
            
            if (m_pWorld->testCollision(m_boundingBox))
            {
                m_position.x = m_boundingBox.m_vPosition.x;
                m_velocity.x = 0.0f;
            }
            else
            {
                m_position.y = m_boundingBox.m_vPosition.y;
                m_position.x = newPosition.x;
                
                m_velocity.y = 0.0f;
                
                m_bFloor = true;
                m_nFloor = m_boundingBox.m_nLastTouched;
            }
        }
        else
            m_position.x = newPosition.x;

        m_boundingBox.m_vPosition.z = newPosition.z;
        if (m_pWorld->testCollision(m_boundingBox))
        {
            // Quake seems to try casting a ray to find the step-iness of a collision
            // So create a ray from our current position + the step height facing down
            // We then teleport to that position and test for collision again
            // If we collide, the step is unsuitable (i.e too tall, maybe ceiling too low)
            // And we go back to the original position
            // If we don't collide, the step is suitable (i.e floor is high enough)
            // And we actually teleport there, treating it as if we've just landed on the floor

            Vector3f rayStart = newPosition + Vector3f(0.0f, STEP_SIZE, 0.0f);
            
            std::pair<Vector3f, Vector3f> ray = CRaycast::cast(m_pWorld, rayStart, Vector3f(0.0f, -1.0f, 0.0f), STEP_SIZE);

            // move the bounding box to the new position
            m_boundingBox.m_vPosition = ray.first;
            
            if (m_pWorld->testCollision(m_boundingBox))
            {
                m_position.z = m_boundingBox.m_vPosition.z;
                m_velocity.z = 0.0f;
            }
            else
            {
                m_position.y = m_boundingBox.m_vPosition.y;
                m_position.z = newPosition.z;
                
                m_velocity.y = 0.0f;
                
                m_bFloor = true;
                m_nFloor = m_boundingBox.m_nLastTouched;
            }
        }
        else
            m_position.x = newPosition.x;
    }

    if (m_bGravity)
    {
        m_velocity += m_gravity * dt;
    }
    
    Vector3f friction;
    if (!m_bFloor)
        friction = m_velocity * 0.7f * -1.0f;
    else
        friction = m_velocity * 0.1f * -1.0f;
    
    m_velocity += friction * dt;
}