#include "inventory.hpp"

CInventory::CInventory( int itemSlots, bool infinite )
{
	m_iItemSlots = itemSlots;
	m_items.reserve( itemSlots );

	for ( int i = 0; i < itemSlots; i++ )
		m_items.push_back( new CBlockItem() );

	m_bInfinite = infinite;
}

CInventory::CInventory( std::vector<CItem *> items, bool infinite )
{
	m_items		 = items;
	m_iItemSlots = items.size();
	m_bInfinite	 = infinite;
}

CInventory::~CInventory() { m_items.clear(); }

CItem *CInventory::Slot( int slot ) { return slot <= m_iItemSlots ? m_items[slot] : nullptr; }

int CInventory::Take( int slot, int amount )
{
	if ( m_bInfinite )
		return amount;

	if ( slot > m_iItemSlots )
		return 0;

	if ( m_items[slot]->GetCount() < amount )
		amount = m_items[slot]->GetCount();

	m_items[slot]->SetCount( m_items[slot]->GetCount() - amount );

	return amount;
}