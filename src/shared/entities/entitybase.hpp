// -- EntityBase class
// The base for ALL entities.

#include "utility/vector.hpp"

#include <string>
#include <map>

#pragma once

using entityId_t = uint32_t; // max 2^32 (4 billion) entities

#define DeclBaseClass(className, baseClassName) \
    using BaseClass = baseClassName; \
    using BaseClass::BaseClass; \
    protected: \
    className() {}; \
    public: \
    static const std::string GetClassName() { return #className; } \
    static const std::string GetBaseClassName() { return #baseClassName; }

class CWorld;

class CEntityBase
{
    protected:
    CEntityBase() {};
public:
    CEntityBase(entityId_t id);
    virtual ~CEntityBase() {};

    virtual void Update(float dt) {};

    void SetPosition(const Vector3f& position);
    void SetPosition(float x, float y, float z);
    void SetRotation(const Vector3f& rotation);
    void SetRotation(float x, float y, float z);
    void SetScale(const Vector3f& scale);
    void SetScale(float x, float y, float z);

    Vector3f GetPosition();
    void GetPosition(float& x, float& y, float& z);
    Vector3f GetRotation();
    void GetRotation(float& x, float& y, float& z);
    Vector3f GetScale();
    void GetScale(float& x, float& y, float& z);

    void SetName(const std::string& name);
    std::string GetName();

    entityId_t GetId();

    void SetWorld(CWorld* world);
    CWorld* GetWorld();

    // key/value runtime data
    void SetData(const std::string& key, const std::string& value);
    void SetData(const std::string& key, int value);
    void SetData(const std::string& key, float value);
    void SetData(const std::string& key, bool value);
    std::string GetData(const std::string& key);
    int GetDataInt(const std::string& key);
    float GetDataFloat(const std::string& key);
    bool GetDataBool(const std::string& key);

    protected:
    Vector3f m_position;
    Vector3f m_rotation;
    Vector3f m_scale;

    std::string m_name;
    entityId_t m_id;

    // key/value runtime data
    std::map<std::string, std::string> m_data;

    CWorld *m_pWorld;
};