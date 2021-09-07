#pragma once

#include "utility/vector.hpp"
#include <vector>

#include <map>

#include "world/block.hpp"

#define SOUNDBUFFERS 1

class CSound
{
  public:
	CSound( const char *path );
	// Listen = location of listener
	// Src = location of sound src
	void Play( CVector src, float pitch, float gain );

	unsigned int id, buffer;
};

class CSoundManager
{
  public:
	CSoundManager();
	~CSoundManager();

	CSound *LoadSound( const char *path );

	std::vector<CSound *> loadedSounds;
	std::map<const char *, CSound *> namedSounds;

	void PlayBreakSound( blocktype_t blockType, CVector pos );
	void PlayPlaceSound( blocktype_t blockType, CVector pos );
};