#include "soundmanager.h"

#include "stb_vorbis.c"

#include <AL/al.h>
#include "physfs.h"

// TODO: Error sound
Sound::Sound(const char* path)
{
	PHYSFS_File *f = PHYSFS_openRead(path);
	int64_t fl = PHYSFS_fileLength(f);
	unsigned char buf[fl];

	int64_t rl = PHYSFS_readBytes(f, &buf, fl);

	if (rl != fl)
	{
		printf("PHYSFS Error!\n%s\n", PHYSFS_getLastError());
	}


	size_t len = 0;
	int channels, rate;
	short *data = NULL;

	unsigned int decode_len = stb_vorbis_decode_memory(reinterpret_cast<uint8*>(&buf), len, &channels, &rate, &data);
	if (decode_len <= 0)
	{
		printf("OGG File %s failed to load!\n", path);
	}

	alGenSources(1, &this->id);
	alSourcef(this->id, AL_PITCH, 1.0);
	alSourcef(this->id, AL_GAIN, 1.0);
	alSource3f(this->id, AL_POSITION, 0, 0, 0);
	alSource3f(this->id, AL_VELOCITY, 0, 0, 0);
	alSourcei(this->id, AL_SOURCE_TYPE, AL_STATIC);
	alSourcei(this->id, AL_LOOPING, 0);

	unsigned int l = decode_len * channels * (sizeof(int16) / sizeof(uint8));
	alGenBuffers(1, &buffer);
	alBufferData(buffer, AL_FORMAT_STEREO16, data, l, rate / 2);
	
	alSourcei(this->id, AL_BUFFER, buffer);
}
void Sound::Play(Vector src, float pitch, float gain)
{
	alSourcef(this->id, AL_PITCH, pitch);
	alSourcef(this->id, AL_GAIN, gain);
	alSource3f(this->id, AL_POSITION, src.x, src.y, src.z);

	alSourcePlay(id);
}

SoundManager::SoundManager()
{
	alGenBuffers(1, &alBuffer);
}
SoundManager::~SoundManager()
{
	for (Sound *s : loadedSounds)
		delete s;
}

Sound *SoundManager::LoadSound(const char* path)
{
	Sound *snd = new Sound(path);
	loadedSounds.push_back(snd);

	return loadedSounds.back();
}
