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
            m_position.y = bbox.m_vPosition.y;
            m_velocity.y = 0.0f;

            m_onGround = true;
            m_floorType = bbox.m_nLastTouched;
        }
        else
            m_position.y = newPosition.y;

        // TODO: handle slopes as I think the casting will be jittery and slow 

        bbox.m_vPosition.x = newPosition.x;
        if (m_world->testCollision(bbox))
        {
            // Quake seems to try casting a ray to find the step-iness of a collision
            // So create a ray from our current position + the step height facing down
            // We then teleport to that position and test for collision again
            // If we collide, the step is unsuitable (i.e too tall, maybe ceiling too low)
            // And we go back to the original position
            // If we don't collide, the step is suitable (i.e floor is high enough)
            // And we actually teleport there, treating it as if we've just landed on the floor

            Vector3f rayStart = newPosition + Vector3f(0.0f, STEP_SIZE, 0.0f);
            
            std::pair<Vector3f, Vector3f> ray = CRaycast::cast(m_world, rayStart, Vector3f(0.0f, -1.0f, 0.0f), STEP_SIZE);

            // move the bounding box to the new position
            bbox.m_vPosition = ray.first;
            
            if (m_world->testCollision(bbox))
            {
                m_position.x = bbox.m_vPosition.x;
                m_velocity.x = 0.0f;
            }
            else
            {
                m_position.y = bbox.m_vPosition.y;
                m_position.x = newPosition.x;
                
                m_velocity.y = 0.0f;
                
                m_onGround = true;
                m_floorType = bbox.m_nLastTouched;
            }
        }
        else
            m_position.x = newPosition.x;

        bbox.m_vPosition.z = newPosition.z;
        if (m_world->testCollision(bbox))
        {
            // Quake seems to try casting a ray to find the step-iness of a collision
            // So create a ray from our current position + the step height facing down
            // We then teleport to that position and test for collision again
            // If we collide, the step is unsuitable (i.e too tall, maybe ceiling too low)
            // And we go back to the original position
            // If we don't collide, the step is suitable (i.e floor is high enough)
            // And we actually teleport there, treating it as if we've just landed on the floor

            Vector3f rayStart = newPosition + Vector3f(0.0f, STEP_SIZE, 0.0f);
            
            std::pair<Vector3f, Vector3f> ray = CRaycast::cast(m_world, rayStart, Vector3f(0.0f, -1.0f, 0.0f), STEP_SIZE);

            // move the bounding box to the new position
            bbox.m_vPosition = ray.first;
            
            if (m_world->testCollision(bbox))
            {
                m_position.z = bbox.m_vPosition.z;
                m_velocity.z = 0.0f;
            }
            else
            {
                m_position.y = bbox.m_vPosition.y;
                m_position.z = newPosition.z;
                
                m_velocity.y = 0.0f;
                
                m_onGround = true;
                m_floorType = bbox.m_nLastTouched;
            }
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

    // Make sure the velocity never reaches more than m_maxSpeed in any direction
    if (m_velocity.x > m_maxSpeed)
        m_velocity.x = m_maxSpeed;
    else if (m_velocity.x < -m_maxSpeed)
        m_velocity.x = -m_maxSpeed;
    
    if (m_velocity.y > m_maxSpeed)
        m_velocity.y = m_maxSpeed;
    else if (m_velocity.y < -m_maxSpeed)
        m_velocity.y = -m_maxSpeed;
    
    if (m_velocity.z > m_maxSpeed)
        m_velocity.z = m_maxSpeed;
    else if (m_velocity.z < -m_maxSpeed)
        m_velocity.z = -m_maxSpeed;
}

void CActorEntity::update(float dt)
{
    CPhysicalEntity::update(dt);

    // TODO: handle movement

    Vector3f forward = m_lookDirection;
    Vector3f right = forward.Cross(Vector3f(0.0f, 1.0f, 0.0f));
    right.y = 0.0f;
    right = right.Normal();
    Vector3f up = Vector3f(0.0f, 1.0f, 0.0f);

    Vector3f move;

    if (m_inForward)
        move += forward;
    if (m_inBackward)
        move -= forward;
    if (m_inLeft)
        move -= right;
    if (m_inRight)
        move += right;
    if (m_inUp)
        move += up;
    if (m_inDown)
        move -= up;


    if (move.Magnitude() > 0.0f)
    {
        move = move.Normal();
        move *= m_maxSpeed;
        m_velocity += move;
    }
}