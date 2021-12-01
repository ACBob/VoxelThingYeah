// item base class

#define ITEMSTACK_MAX 64

#pragma once

class CItem
{
  public:
	CItem();
	~CItem();

	virtual int GetCount();
	virtual void SetCount( int c );

	virtual int GetID();
	virtual void SetID( int id );

  protected:
	// How many of this item does this stack represent
	int m_iCount;

	// The ID of this item
	// In the case of a block, this is the block ID
	int m_iID;
};