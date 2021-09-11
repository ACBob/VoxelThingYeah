#include "soundmanager.hpp"

#include "tomlcpp.hpp"

#include "blockdef.hpp"

#include <random>

#include <AL/al.h>
#include <AL/alc.h>

#include "filesystem.hpp"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

#include "cvar_clientside.hpp"

#include "stb_vorbis.c"

std::vector<CSound *> soundSystem::loadedSounds;
std::map<std::string, CSoundEvent*> soundSystem::soundEvents;

// TODO: Error sound
CSound::CSound( const char *path )
{
	bool bSuccess		= false;
	int64_t iFileLength = 0;

	const uchar_t *soundData = fileSystem::LoadFile(path, iFileLength, bSuccess);

	int channels, rate;
	short *data = NULL;

	unsigned int decode_len =
		stb_vorbis_decode_memory( reinterpret_cast<const uint8 *>( soundData ), iFileLength, &channels, &rate, &data );
	if ( decode_len <= 0 )
	{
		con_error( "OGG File %s failed to load!", path );
	}

	alGenSources( 1, &m_iId );
	alSourcef( m_iId, AL_PITCH, 1.0 );
	alSourcef( m_iId, AL_GAIN, 1.0 );
	alSource3f( m_iId, AL_POSITION, 0, 0, 0 );
	alSource3f( m_iId, AL_VELOCITY, 0, 0, 0 );
	alSourcei( m_iId, AL_SOURCE_TYPE, AL_STATIC );
	alSourcei( m_iId, AL_LOOPING, 0 );

	uint32_t l = decode_len * channels * ( sizeof( int16_t ) / sizeof( uint8_t ) );
	alGenBuffers( 1, &m_iBuffer );
	alBufferData( m_iBuffer, channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, data, l, rate );

	alSourcei( m_iId, AL_BUFFER, m_iBuffer );

	delete soundData;
}
void CSound::Play( CVector src, float pitch, float gain )
{
	alSourcef( m_iId, AL_PITCH, pitch );
	alSourcef( m_iId, AL_GAIN, gain );
	alSource3f( m_iId, AL_POSITION, src.x, src.y, src.z );

	alSourcePlay( m_iId );
}

CSoundEvent::CSoundEvent( std::vector<std::string> sounds, const char *type, float minpitch, float maxpitch )
{
	for (std::string soundName : sounds)
	{
		CSound *s = soundSystem::LoadSound(soundName.c_str());
		m_sounds.push_back(s);
	}

	if (strcmp(type, "block") == 0)
	{
		m_iSoundType = SOUNDTYPE_BLOCK;
	}

	m_fMinPitch = minpitch;
	m_fMaxPitch = maxpitch;
}

void CSoundEvent::Play( CVector pos )
{
	float pitch	= m_fMinPitch + (m_fMaxPitch - m_fMinPitch) * ((rand()) / (float)RAND_MAX);

	int soundIdx = (rand() % m_sounds.size());

	m_sounds.at(soundIdx)->Play(pos, pitch, pitch);
}

ALCdevice *openAlDevice;
ALCcontext *openAlContext;

void soundSystem::Init()
{
	openAlDevice = alcOpenDevice(NULL);
	if (!openAlDevice)
	{
		con_error("OpenAL Initialisation Failed!");
		return;
	}

	openAlContext = alcCreateContext(openAlDevice, NULL);
	if (!openAlContext)
	{
		con_error("OpenAL Intialisation Failed!");
	}
	if (!alcMakeContextCurrent(openAlContext))
	{	
		con_error("Could not make AL context current!");
	}

	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	float ori[] = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0};
	alListenerfv(AL_ORIENTATION, ori);
	alListenerf(AL_GAIN, cl_volume->GetFloat());

	/* Now load the sounds */
	con_info("Loading sounds.toml");

	int64_t fl = 0;
	bool bSuccess = false;
	const uchar_t *soundsToml = fileSystem::LoadFile("sound/sounds.toml", fl, bSuccess);

	toml::Result data = toml::parse((char*)soundsToml);
	
	for (std::string key : data.table->keys())
	{
		auto tbl = data.table->getTable(key);

		auto [ has, type ] = tbl->getString("type");
		if (!has)
			// Assume type
			type = "";

		auto sounds = tbl->getArray("sounds");
		if (!sounds)
			con_warning("Sound %s has no sounds set!", key.c_str());
		
		float minPitch, maxPitch;

		auto pitch = tbl->getArray("pitch");
		auto pA = pitch->getDouble(0);
		if (!pA.first)
			minPitch = 1.0f;
		else
			minPitch = pA.second;
		auto pB = pitch->getDouble(1);
		if (!pB.first)
			maxPitch = 1.0f;
		else
			maxPitch = pB.second;

		soundEvents[key] = new CSoundEvent(*sounds->getStringVector().get(), type.c_str(), minPitch, maxPitch);
	}
}
void soundSystem::UnInit()
{
	for ( CSound *s : loadedSounds )
		delete s;
	for ( auto s : soundEvents )
		delete s.second;
	
	// Shutdown sound
	alcDestroyContext(openAlContext);
	alcCloseDevice(openAlDevice);
}

void soundSystem::SetListener(CVector pos, CVector forward)
{
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	float orient[] = {
		forward.x,
		forward.y,
		forward.z,

		0, 1, 0
	};
	alListenerfv(AL_ORIENTATION, orient);
}

CSound *soundSystem::LoadSound( const char *path )
{
		con_info("Load sound %s", path);
	CSound *snd = new CSound( path );
	loadedSounds.push_back( snd );

	return snd;
}

void soundSystem::PlayBreakSound( blocktype_t blockType, CVector pos )
{
	soundEvents["soundtest"]->Play(pos);
}
void soundSystem::PlayPlaceSound( blocktype_t blockType, CVector pos )
{
	// blockmaterial_t mat = GetBlockMaterial( blockType );
	// float pitch			= 0.5 + ( random() % 15 ) / 10.0f;

	// switch ( mat )
	// {
	// 	case MAT_STONE:
	// 		m_soundEvents["placeStone"]->Play( pos, pitch, 1.0f );
	// 		break;
	// 	case MAT_WOOD:
	// 		m_soundEvents["placeWood"]->Play( pos, pitch, 1.0f );
	// 		break;
	// 	case MAT_LOOSE:
	// 		m_soundEvents["placeLoose"]->Play( pos, pitch, 1.0f );
	// 		break;
	// 	case MAT_GLASS:
	// 		m_soundEvents["placeGlass"]->Play( pos, pitch, 1.0f );
	// 		break;
	// 	case MAT_ORGANIC:
	// 		m_soundEvents["placeOrganic"]->Play( pos, pitch, 1.0f );
	// 		break;
	// }
}