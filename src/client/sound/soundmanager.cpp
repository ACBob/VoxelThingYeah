#include "soundmanager.hpp"

#include "stb_vorbis.c"

#include "world/blockdef.hpp"

#include <random>

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


	int channels, rate;
	short *data = NULL;

	unsigned int decode_len = stb_vorbis_decode_memory(reinterpret_cast<uint8*>(&buf), fl, &channels, &rate, &data);
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

	uint32_t l = decode_len * channels * (sizeof(int16_t) / sizeof(uint8_t));
	alGenBuffers(1, &buffer);
	alBufferData(buffer, channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, data, l, rate);

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
	namedSounds["placeStone"] = LoadSound("sound/placestone.ogg");
	namedSounds["breakStone"] = LoadSound("sound/breakstone.ogg");

	namedSounds["placeWood"] = LoadSound("sound/placewood.ogg");
	namedSounds["breakWood"] = LoadSound("sound/breakwood.ogg");

	namedSounds["placeLoose"] = LoadSound("sound/placeloose.ogg");
	namedSounds["breakLoose"] = LoadSound("sound/breakloose.ogg");

	namedSounds["placeGlass"] = namedSounds["placeStone"];
	namedSounds["breakGlass"] = LoadSound("sound/breakglass.ogg");
	
	namedSounds["placeOrganic"] = LoadSound("sound/placeorganic.ogg");
	namedSounds["breakOrganic"] = LoadSound("sound/breakorganic.ogg");
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


void SoundManager::PlayBreakSound(blocktype_t blockType, Vector pos)
{
	blockmaterial_t mat = GetBlockMaterial(blockType);
	float pitch = 0.5 + (random() % 15) / 10.0f;

	switch (mat)
	{
		case MAT_STONE:
			namedSounds["breakStone"]->Play(pos, pitch, 1.0f);
		break;
		case MAT_WOOD:
			namedSounds["breakWood"]->Play(pos, pitch, 1.0f);
		break;
		case MAT_LOOSE:
			namedSounds["breakLoose"]->Play(pos, pitch, 1.0f);
		break;
		case MAT_GLASS:
			namedSounds["breakGlass"]->Play(pos, pitch, 1.0f);
		break;
		case MAT_ORGANIC:
			namedSounds["breakOrganic"]->Play(pos, pitch, 1.0f);
		break;
	}
}
void SoundManager::PlayPlaceSound(blocktype_t blockType, Vector pos)
{
	blockmaterial_t mat = GetBlockMaterial(blockType);
	float pitch = 0.5 + (random() % 15) / 10.0f;

	switch (mat)
	{
		case MAT_STONE:
			namedSounds["placeStone"]->Play(pos, pitch, 1.0f);
		break;
		case MAT_WOOD:
			namedSounds["placeWood"]->Play(pos, pitch, 1.0f);
		break;
		case MAT_LOOSE:
			namedSounds["placeLoose"]->Play(pos, pitch, 1.0f);
		break;
		case MAT_GLASS:
			namedSounds["placeGlass"]->Play(pos, pitch, 1.0f);
		break;
		case MAT_ORGANIC:
			namedSounds["placeOrganic"]->Play(pos, pitch, 1.0f);
		break;
	}
}