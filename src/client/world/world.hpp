#include "shared/world/world.hpp"

#include "client/rendering/material.hpp"
#include "client/rendering/model.hpp"

class CClientChunk;

class CClientWorld : public CWorld
{
    public:
        CClientWorld();
        ~CClientWorld();

        void render();

        std::map<Vector3i, CClientChunk*> m_chunks;
};