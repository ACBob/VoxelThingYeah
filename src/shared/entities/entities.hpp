// -- Entity System
// Inspired by Cube 2 Sauerbraten

#include "utility/vector.hpp"

#include "world/physics.hpp"

#pragma once

#define MAX_ENTITIES 65536
#define MAX_ENTITY_TYPES 256
#define MAX_ENTITY_NAME_LENGTH 32

#define ENTITY_ATTRIBUTE(type, name) \
    protected: \
        type m_##name; \
    public: \
        type get##name() const { return m_##name; } \
        void set##name(type name) { m_##name = name; }

#define ENTITY_ATTRIBUTE_NO_SET(type, name) \
    protected: \
        type m_##name; \
    public: \
        type get##name() const { return m_##name; }

#define STUB_ATTRIBUTE(type, name) \
    public: \
        virtual type get##name() const { return type(); } \
        virtual void set##name(type name) { }

#define ENTITY_MODEL(modelname) \
    public: \
        virtual bool hasModel() { return true; } \
        virtual const char *getModelName() { return modelname; }

// RAW data needed to reconstruct entities
// Stored when saving the map
struct SRawEntity {
    Vector3f position; // Position in-world
    char name[MAX_ENTITY_NAME_LENGTH]; // Name of the entity
    char *properties; // A series of bytes that represent properties, parsed by individual entities
    unsigned char type; // Type of the entity
};

enum EEntityFlags : unsigned char {
    EF_NONE = 0,
    EF_DEAD = 1 << 0, // Should be removed from the world
    EF_SPAWNED = 1 << 1, // Has been spawned in the world, process it
};

// Forward decls.
class CWorld;

// An entity at run-time
class CEntity : public SRawEntity {
protected:
    ENTITY_ATTRIBUTE(CEntity*, parent); // Parent entity
    ENTITY_ATTRIBUTE(Vector3f, position); // Position in-world
    ENTITY_ATTRIBUTE(unsigned char, flags); // Flags

    ENTITY_ATTRIBUTE(CWorld*, world); // World this entity is in

    STUB_ATTRIBUTE(Vector3f, velocity); // Velocity in-world
    STUB_ATTRIBUTE(Vector3f, rotation); // Rotation in-world
    STUB_ATTRIBUTE(Vector3f, scale); // Scale in-world

public:
    CEntity() : m_parent(NULL) {
        position = Vector3f(0, 0, 0);
        properties = NULL;
        type = 0;
    }

    bool isSpawned() {
        return (m_flags & EF_SPAWNED) == EF_SPAWNED;
    }
    void setSpawned() {
        m_flags |= EF_SPAWNED;
    }
    void clearSpawned() {
        m_flags &= ~EF_SPAWNED;
    }

    void kill() {
        m_flags |= EF_DEAD;
    }
    bool isDead() {
        return (m_flags & EF_DEAD) == EF_DEAD;
    }

    const char* getName() {
        return name;
    }

    const char* className() {
        return "CEntity";
    }

    virtual void update(float dt) {
    }

    virtual bool hasModel() {
        return false;
    }
    virtual const char *getModelName() {
        return "";
    }
};

class CPhysicalEntity : public CEntity // An entity affected by physics
{
protected:
    ENTITY_ATTRIBUTE(Vector3f, velocity); // Velocity in-world
    ENTITY_ATTRIBUTE(Vector3f, rotation); // Rotation in-world
    ENTITY_ATTRIBUTE(Vector3f, scale); // Scale in-world

    ENTITY_ATTRIBUTE(float, mass); // Mass of the entity
    ENTITY_ATTRIBUTE(float, maxSpeed); // Maximum speed of the entity, in m/s

    ENTITY_ATTRIBUTE(bool, gravity); // Gravity enabled
    ENTITY_ATTRIBUTE(bool, collision); // Collision enabled
    ENTITY_ATTRIBUTE_NO_SET(bool, onGround); // Is the entity on the ground?
    ENTITY_ATTRIBUTE_NO_SET(uint16_t, floorType); // Type of the floor

public:
    CPhysicalEntity() : CEntity() {
        m_velocity = Vector3f(0, 0, 0);
        m_rotation = Vector3f(0, 0, 0);
        m_maxSpeed = 0;
        m_mass = 0;
    }

    void reset() {
        m_velocity = Vector3f(0, 0, 0);
        m_rotation = Vector3f(0, 0, 0);
        m_maxSpeed = 0;
        m_mass = 0;
    }

    virtual void update(float dt);

    const char* className() {
        return "CPhysicalEntity";
    }

    virtual const CBoundingBox getBoundingBox() {
        return CBoundingBox(Vector3f(0, 0, 0), Vector3f(0, 0, 0)) * m_scale;
    }
};

class CActorEntity : public CPhysicalEntity // An entity that can recieve input
{
protected:
    ENTITY_ATTRIBUTE(int, hearts); // Health, measured in half-hearts
    ENTITY_ATTRIBUTE(int, maxHearts); // Maximum health, measured in half-hearts

    ENTITY_ATTRIBUTE(bool, immortal); // Whether the entity is immortal

public:
    bool m_inForward, m_inBackward, m_inLeft, m_inRight, m_inUp, m_inBack; // Inputs
    bool m_inAttack, m_inJump, m_inCrouch; // Inputs

    CActorEntity() : CPhysicalEntity() {
        m_inForward = m_inBackward = m_inLeft = m_inRight = m_inUp = m_inBack = false;
        m_inAttack = m_inJump = m_inCrouch = false;
        m_hearts = m_maxHearts = 0;
        m_immortal = false;
    }

    void reset() {
        CPhysicalEntity::reset();
        m_inForward = m_inBackward = m_inLeft = m_inRight = m_inUp = m_inBack = false;
        m_inAttack = m_inJump = m_inCrouch = false;
        m_hearts = m_maxHearts = 0;
        m_immortal = false;
    }

    void damage(int amount) {
        if (m_immortal) return;
        m_hearts -= amount;
        if (m_hearts <= 0) {
            m_hearts = 0;
            kill();
        }
    }
    void heal(int amount) {
        if (m_immortal) return;
        m_hearts += amount;
        if (m_hearts > m_maxHearts) {
            m_hearts = m_maxHearts;
        }
    }

    virtual void update(float dt);

    const char* className() {
        return "CActorEntity";
    }
};

#define DeclareEntity(newClassName, entityBase) \
    using BaseClass = entityBase;\
    const char* className() { return #newClassName; }

enum {
    ENT_PLAYER = 1,
    ENT_CAMERA = 2
};

// Regardless of game, these entities are always present
class CPlayerEntity : public CActorEntity {
    ENTITY_MODEL("PlayerModel");
public:
    DeclareEntity(CPlayerEntity, CActorEntity);

    CPlayerEntity() : CActorEntity() {
        m_hearts = m_maxHearts = 20;
        type = ENT_PLAYER;
    }

    void reset() {
        BaseClass::reset();
        m_hearts = m_maxHearts = 20;
    }

    virtual const CBoundingBox getBoundingBox() {
        return CBoundingBox(Vector3f(-0.5, -0.5, -0.5), Vector3f(0.5, 0.5, 0.5)) * m_scale;
    }
};

class CCameraEntity : public CEntity {
protected:
    ENTITY_ATTRIBUTE(float, fov); // Field of view
    ENTITY_ATTRIBUTE(Vector3f, rotation); // Rotation in-world

public:
    DeclareEntity(CCameraEntity, CEntity);

    CCameraEntity() : CEntity() {
        m_fov = 90;
        m_rotation = Vector3f(0, 0, 0);
        type = ENT_CAMERA;
    }

    void reset() {
        m_fov = 90;
        m_rotation = Vector3f(0, 0, 0);
    }
};