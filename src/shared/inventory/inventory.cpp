#include "inventory.hpp"

CInventory::CInventory(int itemSlots)
{
	m_iItemSlots = itemSlots;
	m_pItems = new CItem[itemSlots];
}

CInventory::~CInventory()
{
	delete[] m_pItems;
}