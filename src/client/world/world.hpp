#include "shared/world/world.hpp"

#include "render/material.hpp"
#include "render/model.hpp"

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

        uint16_t getID( int x, int y, int z );
        void setID( int x, int y, int z, uint16_t id );

        uint16_t getMeta( int x, int y, int z );
        void setMeta( int x, int y, int z, uint16_t meta );

        void get(int x, int y, int z, uint16_t &id, uint16_t &meta);
        void set(int x, int y, int z, uint16_t id, uint16_t meta);
        uint32_t get(int x, int y, int z);
        void set(int x, int y, int z, uint32_t data);

        void render();
        void update(float dt);

    protected:
        std::map<Vector3i, CClientChunk*> m_chunks;
};