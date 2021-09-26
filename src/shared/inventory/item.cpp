#include "item.hpp"

CItem::CItem() : m_iCount(0) {}
CItem::~CItem() {}

int CItem::GetCount()
{
	return m_iCount;
}
void CItem::SetCount(int c)
{
	m_iCount = c <= ITEMSTACK_MAX ? c : ITEMSTACK_MAX;
}