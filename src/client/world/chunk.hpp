#include "shared/world/chunk.hpp"

#include "render/model.hpp"

class CClientChunk : public CChunk
{

    public:
        CClientChunk( int x, int y, int z, int sizeX, int sizeY, int sizeZ );
        ~CClientChunk();

        void constructModel();
        void render();

        rendering::CModel m_model;

};