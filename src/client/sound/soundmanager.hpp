#pragma once

#include "utility/vector.hpp"
#include <vector>

#include <map>

#include "world/block.hpp"

#define SOUNDBUFFERS 1

// TODO: namespace-ify like the other managers

class CSound
{
  public:
	CSound( const char *path );
	// Listen = location of listener
	// Src = location of sound src
	void Play( CVector src, float pitch, float gain );

	unsigned int m_iId, m_iBuffer;
};

class CSoundManager
{
  public:
	CSoundManager();
	~CSoundManager();

	CSound *LoadSound( const char *path );

	std::vector<CSound *> m_loadedSounds;
	std::map<const char *, CSound *> m_namedSounds;

	void PlayBreakSound( blocktype_t blockType, CVector pos );
	void PlayPlaceSound( blocktype_t blockType, CVector pos );
};