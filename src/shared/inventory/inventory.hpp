// Something that can hold items

#include "item.hpp"
// Nothing used from here but do it so that everyone including us knows what a block item is
#include "blockitem.hpp"

#include <vector>

#pragma once

class CInventory
{
	public:
		CInventory(int slots);
		~CInventory();

		// Returns the item at slot
		CItem *Slot(int slot);

		// itemSlots big
		std::vector<CItem*> m_items;
		int m_iItemSlots; // The amount of slots we have
};