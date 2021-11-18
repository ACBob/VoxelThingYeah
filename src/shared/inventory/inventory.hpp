// Something that can hold items

#include "item.hpp"
// Nothing used from here but do it so that everyone including us knows what a block item is
#include "blockitem.hpp"

#include <vector>

#pragma once

class CInventory
{
  public:
	CInventory( int slots, bool infinite = false );
	CInventory( std::vector<CItem *> items, bool infinite = false );
	~CInventory();

	// Returns the item at slot
	CItem *Slot( int slot );

	// Returns the count of items we were able to take
	// Takes from our inventory
	// Reference to slot should still be valid, albeit less than in number
	int Take( int slot, int count );

	// itemSlots big
	std::vector<CItem *> m_items;
	int m_iItemSlots; // The amount of slots we have

	bool m_bInfinite = false; // If the items in this inventory are infinite
};