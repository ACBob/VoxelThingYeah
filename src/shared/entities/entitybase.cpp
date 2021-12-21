#include "entitybase.hpp"

CEntityBase::CEntityBase(entityId_t id, CWorld *pWorld)
{
    m_id = id;
    m_parent = nullptr;
    m_pWorld = pWorld;

    m_scale = Vector3f(1.0f, 1.0f, 1.0f);
}

void CEntityBase::Update(float dt)
{
    if (m_parent != nullptr)
    {
        m_position = m_parent->GetPosition() + m_relativePosition;
        m_rotation = m_parent->GetRotation() + m_relativeRotation;
        m_scale = m_parent->GetScale() * m_relativeScale;
    }
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

void CEntityBase::SetLocalPosition(const Vector3f& position)
{
    m_relativePosition = position;
}
void CEntityBase::SetLocalPosition(float x, float y, float z)
{
    m_relativePosition.x = x;
    m_relativePosition.y = y;
    m_relativePosition.z = z;
}
void CEntityBase::SetLocalRotation(const Vector3f& rotation)
{
    m_relativeRotation = rotation;
}
void CEntityBase::SetLocalRotation(float x, float y, float z)
{
    m_relativeRotation.x = x;
    m_relativeRotation.y = y;
    m_relativeRotation.z = z;
}
void CEntityBase::SetLocalScale(const Vector3f& scale)
{
    m_relativeScale = scale;
}
void CEntityBase::SetLocalScale(float x, float y, float z)
{
    m_relativeScale.x = x;
    m_relativeScale.y = y;
    m_relativeScale.z = z;
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

entityId_t CEntityBase::GetId()
{
    return m_id;
}

void CEntityBase::AddChild(CEntityBase* child)
{
    if (m_children.size() > 0)
    {
        for (auto it = m_children.begin(); it != m_children.end(); ++it)
        {
            if ((*it)->GetId() == child->GetId())
            {
                return;
            }
        }
    }

    m_children.push_back(child);
    child->SetParent(this);
}
void CEntityBase::RemoveChild(CEntityBase* child)
{
    for (auto it = m_children.begin(); it != m_children.end(); ++it)
    {
        if (*it == child)
        {
            m_children.erase(it);
            child->SetParent(nullptr);
            break;
        }
    }
}
void CEntityBase::DestroyChild(CEntityBase* child)
{
    RemoveChild(child);
    delete child;
}
std::vector<CEntityBase*> CEntityBase::GetChildren()
{
    return m_children;
}

CEntityBase* CEntityBase::GetParent()
{
    return m_parent;
}

void CEntityBase::SetParent(CEntityBase* parent)
{
    m_parent = parent;
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