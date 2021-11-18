#pragma once

#include "blockbase.hpp"

#include "sound/soundmanager.hpp"

class CBlockSoundMaker final : public CBlockBase
{
	using Super = CBlockBase;

  public:
	using Super::Super;

	virtual bool CanBeUsed() const { return true; };

	virtual void OnUse( CChunk *pChunk, Vector3f vPosition, CEntityBase *pUser ) const
	{
		soundSystem::PlaySoundEvent( "block.soundmaker.use", vPosition );
	};

	virtual BLOCKMATERIAL GetMaterial() const { return MAT_FABRIC; };
};