#include "shared/world/chunk.hpp"

#include "client/rendering/model.hpp"

class CClientChunk final : public CChunk
{

    public:
        CClientChunk( int x, int y, int z, int sizeX, int sizeY, int sizeZ );
        ~CClientChunk();

        void constructModel();
        void render();

        CModel m_model;

};