#include "inventory.hpp"

CInventory::CInventory(int itemSlots)
{
	m_iItemSlots = itemSlots;
	m_items.reserve(itemSlots);

	for (int i = 0; i < itemSlots; i++)
		m_items.push_back(new CBlockItem());
}

CInventory::~CInventory()
{
	m_items.clear();
}

CItem *CInventory::Slot(int slot)
{
	return slot <= m_iItemSlots ? m_items[slot] : nullptr;
}