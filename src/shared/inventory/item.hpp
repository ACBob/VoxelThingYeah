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

  private:
	// How many of this item does this stack represent
	int m_iCount;
};