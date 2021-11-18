#pragma once

#include "utility/vector.hpp"
#include <vector>

#include <map>

#include "world/block.hpp"
#include "world/world.hpp"

#include "shared/logging.hpp"

#ifdef MEEGREEF_ENABLE_OPENAL

	// Some extra buffers for OpenAL
	// So we can play sounds without having to wait for the last one to finish
	#define SOURCES_PER_SOUND 4

namespace soundSystem
{
	class CSound
	{
	  public:
		CSound( const char *path );
		// Listen = location of listener
		// Src = location of sound src
		void Play( Vector3f src, float pitch, float gain );

		unsigned int m_iSources[SOURCES_PER_SOUND];
		unsigned int m_iBuffer;
	};

	class CSoundEvent
	{
		enum SOUNDTYPE {
			SOUNDTYPE_BLOCK	 = 0x0,
			SOUNDTYPE_MUSIC	 = 0x1,
			SOUNDTYPE_GUI	 = 0x2,
			SOUNDTYPE_ENTITY = 0x3,
		};

	  public:
		CSoundEvent( std::vector<std::string> sounds, const char *type, float minpitch, float maxpitch );

		void Play( Vector3f src );

		std::vector<CSound *> m_sounds;

		float m_fMinPitch = 1.0f;
		float m_fMaxPitch = 1.0f;

		unsigned int m_iSoundType = 0;
	};

	void Init();
	void UnInit();

	void SetListener( CWorld *wlrd, Vector3f pos, Vector3f forward, Vector3f vel );

	CSound *LoadSound( const char *path );

	extern std::map<std::string, CSound *> loadedSounds;
	extern std::map<std::string, CSoundEvent *> soundEvents;

	void PlayBreakSound( BLOCKID blockType, Vector3f pos );
	void PlayPlaceSound( BLOCKID blockType, Vector3f pos );
	void PlayStepSound( BLOCKID blockType, Vector3f pos );

	void PlaySoundEvent( CSoundEvent *event, Vector3f pos );
	void PlaySoundEvent( const char *eventName, Vector3f pos );
} // namespace soundSystem

// Stubs when sound is disabled
#else

namespace soundSystem
{
	class CSound
	{
	  public:
		CSound( const char *path ){};
		// Listen = location of listener
		// Src = location of sound src
		void Play( Vector3f src, float pitch, float gain ){};
	};

	class CSoundEvent
	{
		enum SOUNDTYPE {
			SOUNDTYPE_BLOCK = 0x0,
		};

	  public:
		CSoundEvent( std::vector<std::string> sounds, const char *type, float minpitch, float maxpitch ){};

		void Play( Vector3f src ){};

		std::vector<CSound *> m_sounds;

		float m_fMinPitch = 1.0f;
		float m_fMaxPitch = 1.0f;

		unsigned int m_iSoundType = 0;
	};

	inline void Init() { con_warning( "Meegreef compiled without audio support." ); };
	inline void UnInit(){};

	inline void SetListener( CWorld *wlrd, Vector3f pos, Vector3f forward, Vector3f vel ){};

	inline CSound *LoadSound( const char *path ){};

	extern std::vector<CSound *> loadedSounds;
	extern std::map<std::string, CSoundEvent *> soundEvents;

	inline void PlayBreakSound( BLOCKID blockType, Vector3f pos ){};
	inline void PlayPlaceSound( BLOCKID blockType, Vector3f pos ){};
	inline void PlayStepSound( BLOCKID blockType, Vector3f pos ){};

	inline void PlaySoundEvent( CSoundEvent *event, Vector3f pos ){};
	inline void PlaySoundEvent( const char *eventName, Vector3f pos ){};
} // namespace soundSystem

#endif

using CSound	  = soundSystem::CSound;
using CSoundEvent = soundSystem::CSoundEvent;