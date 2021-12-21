#include "shared/world/world.hpp"

#include "render/material.hpp"
#include "render/model.hpp"

#include "client/entities/entitybase.hpp"

class CClientChunk;

class CClientWorld final : public CWorld
{
    public:
        CClientWorld();
        ~CClientWorld();

        CClientChunk *getChunk( int x, int y, int z );
        CClientChunk *getChunk( const Vector3i &coord );

        CClientChunk *createChunk( int x, int y, int z );
        CClientChunk *createChunk( const Vector3i &coord );

        CClientChunk *getOrCreateChunk( int x, int y, int z );
        CClientChunk *getOrCreateChunk( const Vector3i &coord );

        CClientChunk *getChunkWorldPos( int x, int y, int z );
        CClientChunk *getChunkWorldPos( const Vector3i &coord );

        void render();
        void update(float dt);

        CClientEntityBase *getEntity( int id );
        CClientEntityBase *getEntity( const std::string &name );
        std::vector<CClientEntityBase *> getEntitiesByName( const std::string &name );

        template <typename T>
        T *createEntity()
        {
            // TODO: better id management
            T *e = new T( m_entities.size(), this );
            m_entities.push_back( e );
            return e;
        }

    protected:
        std::map<Vector3i, CClientChunk*> m_chunks;
        std::vector<CClientEntityBase*> m_entities;
};