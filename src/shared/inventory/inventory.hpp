// Something that can hold items

#include "item.hpp"

class CInventory
{
	public:
		CInventory(int slots);
		~CInventory();

		// itemSlots big
		CItem *m_pItems;
		int m_iItemSlots; // The amount of slots we have
};