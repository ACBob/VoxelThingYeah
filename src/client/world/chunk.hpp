#include "shared/world/chunk.hpp"

#include "client/rendering/model.hpp"

class CClientChunk : public CChunk
{

    public:
        CClientChunk();
        ~CClientChunk();

        void ConstructModel();

        CModel m_model;

};