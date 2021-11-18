// Server-side sound manager.
// Pretty much just has some functions which send to the client.

#pragma once

#include "utility/vector.hpp"

#include <map>

#include "world/block.hpp"
#include "world/world.hpp"

#include "network/server.hpp"

#include "shared/logging.hpp"

#define SOUND_MAX_DISTANCE 128

namespace soundSystem
{
	void Init();
	void UnInit();

	void PlayBreakSound( BLOCKID blockType, Vector3f pos );
	void PlayPlaceSound( BLOCKID blockType, Vector3f pos );
	void PlayStepSound( BLOCKID blockType, Vector3f pos );

	void PlaySoundEvent( const char *eventName, Vector3f pos );

	extern CNetworkServer *server;
} // namespace soundSystem