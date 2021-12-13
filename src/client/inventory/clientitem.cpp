#include "clientitem.hpp"

CClientItem::CClientItem() : CItem()
{ }

CClientItem::~CClientItem()
{ }

CTexture *CClientItem::GetTexture()
{
    return materialSystem::LoadTexture("items.png");
}

Vector4f CClientItem::GetUV()
{
	return Vector4f( (float)m_iID / 16.0f, 0.0f, (float)m_iID / 16.0f + 1.0f / 16.0f, 1.0f / 16.0f );
}
CColour CClientItem::GetTint() { return (uint16_t)0xFFFF; }