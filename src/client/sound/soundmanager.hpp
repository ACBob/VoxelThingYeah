#pragma once

#include "utility/vector.hpp"
#include <vector>

#include <map>

#include "world/block.hpp"

#define SOUNDBUFFERS 1

namespace soundSystem
{
	class CSound
	{
	public:
		CSound( const char *path );
		// Listen = location of listener
		// Src = location of sound src
		void Play( CVector src, float pitch, float gain );

		unsigned int m_iId, m_iBuffer;
	};

	class CSoundEvent
	{
		enum SOUNDTYPE
		{
			SOUNDTYPE_BLOCK = 0x0,
		};

		public:
			CSoundEvent( std::vector<std::string> sounds, const char *type, float minpitch, float maxpitch );

			void Play( CVector src );

			std::vector<CSound*> m_sounds;

			float m_fMinPitch = 1.0f;
			float m_fMaxPitch = 1.0f;

			unsigned int m_iSoundType = 0;
	};

	void Init();
	void UnInit();

	void SetListener( CVector pos, CVector forward, CVector vel );

	CSound *LoadSound( const char *path );

	extern std::vector<CSound *> loadedSounds;
	extern std::map<std::string, CSoundEvent*> soundEvents;

	void PlayBreakSound( blocktype_t blockType, CVector pos );
	void PlayPlaceSound( blocktype_t blockType, CVector pos );
}

using CSound = soundSystem::CSound;
using CSoundEvent = soundSystem::CSoundEvent;