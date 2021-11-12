// item base class

#define ITEMSTACK_MAX 64

#pragma once

#ifdef CLIENTEXE
#include "rendering/texturemanager.hpp"
#include "vector.hpp"
#endif


class CItem
{
  public:
	CItem();
	~CItem();

	virtual int GetCount();
	virtual void SetCount( int c );

	virtual int GetID();

#ifdef CLIENTEXE
	virtual CTexture* GetTexture();
	virtual Vector4f GetUV();
#endif

  protected:
	// How many of this item does this stack represent
	int m_iCount;

	// The ID of this item
	// In the case of a block, this is the block ID
	int m_iID;
};