#pragma once

#include "utility/vector.h"
#include <vector>

#define SOUNDBUFFERS 1

class Sound
{
	public:
		Sound(const char* path);
		// Listen = location of listener
		// Src = location of sound src
		void Play(Vector src, float pitch, float gain);

		unsigned int id, buffer;
};

class SoundManager
{
	public:
		SoundManager();
		~SoundManager();

		Sound* LoadSound(const char* path);

		std::vector<Sound*> loadedSounds;

		// AL Sound buffer
		unsigned int alBuffer;
};