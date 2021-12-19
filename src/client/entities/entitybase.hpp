// Client-side Entity Base
// Just adds a model to the entity.

#include "shared/entities/entitybase.hpp"

#include "render/model.hpp"

#pragma once

class CClientEntityBase : virtual public CEntityBase
{
    DeclBaseClass(CClientEntityBase, CEntityBase);

    public:
    CClientEntityBase(entityId_t id);
    virtual ~CClientEntityBase() {};

    void SetModel(const std::string& model);

    void Render();

    protected:
    CModel m_model;
};