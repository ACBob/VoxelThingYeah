#include "item.hpp"

CItem::CItem() : m_iCount( 0 ) {}
CItem::~CItem() {}

int CItem::GetCount() { return m_iCount; }
void CItem::SetCount( int c ) { m_iCount = c <= ITEMSTACK_MAX ? c : ITEMSTACK_MAX; }

int CItem::GetID() { return m_iID; }
void CItem::SetID( int id ) { m_iID = id; }

#ifdef CLIENTEXE
CTexture* CItem::GetTexture() { return materialSystem::LoadTexture("items.png"); }
Vector4f CItem::GetUV() { return Vector4f( (float)m_iID / 16.0f, 0.0f, (float)m_iID / 16.0f + 1.0f / 16.0f, 1.0f / 16.0f ); }
#endif