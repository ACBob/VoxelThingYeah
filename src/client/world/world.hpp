#include "shared/world/world.hpp"

#include "client/rendering/material.hpp"
#include "client/rendering/model.hpp"

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

        std::map<Vector3i, CClientChunk*> m_chunks;
};