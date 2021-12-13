#include "shared/inventory/item.hpp"

#include "rendering/texturemanager.hpp"
#include "vector.hpp"
#include "colour.hpp"

class CClientItem : public CItem
{
    public:
        CClientItem();
        ~CClientItem();

        virtual CTexture *GetTexture();
        virtual Vector4f GetUV();
        virtual CColour GetTint();
};