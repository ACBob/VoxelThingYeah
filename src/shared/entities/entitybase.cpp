#include "entitybase.hpp"

CEntityBase::CEntityBase(entityId_t id)
{
    m_id = id;
}

void CEntityBase::SetPosition(const Vector3f& position)
{
    m_position = position;
}
void CEntityBase::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}
void CEntityBase::SetRotation(const Vector3f& rotation)
{
    m_rotation = rotation;
}
void CEntityBase::SetRotation(float x, float y, float z)
{
    m_rotation.x = x;
    m_rotation.y = y;
    m_rotation.z = z;
}
void CEntityBase::SetScale(const Vector3f& scale)
{
    m_scale = scale;
}
void CEntityBase::SetScale(float x, float y, float z)
{
    m_scale.x = x;
    m_scale.y = y;
    m_scale.z = z;
}

Vector3f CEntityBase::GetPosition()
{
    return m_position;
}
void CEntityBase::GetPosition(float& x, float& y, float& z)
{
    x = m_position.x;
    y = m_position.y;
    z = m_position.z;
}
Vector3f CEntityBase::GetRotation()
{
    return m_rotation;
}
void CEntityBase::GetRotation(float& x, float& y, float& z)
{
    x = m_rotation.x;
    y = m_rotation.y;
    z = m_rotation.z;
}
Vector3f CEntityBase::GetScale()
{
    return m_scale;
}
void CEntityBase::GetScale(float& x, float& y, float& z)
{
    x = m_scale.x;
    y = m_scale.y;
    z = m_scale.z;
}

void CEntityBase::SetWorld(CWorld* world)
{
    m_pWorld = world;
}
CWorld* CEntityBase::GetWorld()
{
    return m_pWorld;
}

void CEntityBase::SetName(const std::string& name)
{
    m_name = name;
}
std::string CEntityBase::GetName()
{
    return m_name;
}

void CEntityBase::SetData(const std::string& key, const std::string& value)
{
    m_data[key] = value;
}

#define ENTITY_SET_DATA(type) \
    void CEntityBase::SetData(const std::string& key, type value) \
    { \
        m_data[key] = std::to_string(value); \
    }

ENTITY_SET_DATA(int);
ENTITY_SET_DATA(float);
ENTITY_SET_DATA(bool);

std::string CEntityBase::GetData(const std::string& key)
{
    if (m_data.find(key) == m_data.end())
        return "";
    return m_data[key];
}

int CEntityBase::GetDataInt(const std::string& key)
{
    if (m_data.find(key) == m_data.end())
        return 0;
    return std::stoi(m_data[key]);
}

float CEntityBase::GetDataFloat(const std::string& key)
{
    if (m_data.find(key) == m_data.end())
        return 0.0f;
    return std::stof(m_data[key]);
}

bool CEntityBase::GetDataBool(const std::string& key)
{
    if (m_data.find(key) == m_data.end())
        return false;
    return m_data[key] == "true";
}