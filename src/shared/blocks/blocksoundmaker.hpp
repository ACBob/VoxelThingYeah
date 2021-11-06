#pragma once

#include "blockbase.hpp"

#ifdef CLIENTEXE
#include "sound/soundmanager.hpp"
#endif

class CBlockSoundMaker final : public CBlockBase
{
    using Super = CBlockBase;
    public:
    
    using Super::Super;

	virtual bool CanBeUsed() const {
		return true;
	};

#ifdef CLIENTEXE
    virtual void OnUse( CChunk *pChunk, CVector vPosition, CEntityBase *pUser ) const
    {
        soundSystem::PlaySoundEvent( "block.soundmaker.use", vPosition );
    };
#endif

    virtual BLOCKMATERIAL GetMaterial( ) const {
		return MAT_FABRIC;
	};
};