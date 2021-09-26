#include "item.hpp"
#include "assorted.hpp"

#include "../blockdef.hpp"

class CBlockItem : public CItem
{
	DeclBaseClass( CItem );

	public:
		CBlockItem();
		~CBlockItem();

		// AIR is functionally the same as 0.
		blocktype_t m_iBlockType;

		int GetCount();
};