#include "entitybase.hpp"

CClientEntityBase::CClientEntityBase(entityId_t id, CWorld *pWorld) : BaseClass(id, pWorld)
{
}

void CClientEntityBase::SetModel(const std::string& model)
{
    m_model.LoadOBJ(model);
}

void CClientEntityBase::Render()
{
    m_model.Render(
        m_position,
        m_rotation,
        {1.0f, 1.0f, 1.0f}
    );
}