#include "soundmanager.hpp"

#include "toml.hpp"

#include "blockdef.hpp"

#include <random>

#include <AL/al.h>
#include <AL/alc.h>

#include "filesystem.hpp"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

#include "cvar_clientside.hpp"

#include "stb_vorbis.c"

std::vector<CSound *> soundSystem::m_loadedSounds;
std::map<const char *, CSound *> soundSystem::m_namedSounds;

// TODO: Error sound
CSound::CSound( const char *path )
{
	bool bSuccess		= false;
	int64_t iFileLength = 0;

	const uchar_t *soundData = fileSystem::LoadFile(path, iFileLength, bSuccess);

	int channels, rate;
	short *data = NULL;

	unsigned int decode_len =
		stb_vorbis_decode_memory( reinterpret_cast<uint8 *>( &soundData ), iFileLength, &channels, &rate, &data );
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

void soundSystem::Init()
{
	ALCdevice *openAlDevice = alcOpenDevice(NULL);
	if (!openAlDevice)
	{
		con_error("OpenAL Initialisation Failed!");
		return;
	}

	ALCcontext *openAlContext = alcCreateContext(openAlDevice, NULL);
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
}
void soundSystem::UnInit()
{
	for ( CSound *s : m_loadedSounds )
		delete s;
}

CSound *soundSystem::LoadSound( const char *path )
{
	CSound *snd = new CSound( path );
	m_loadedSounds.push_back( snd );

	return snd;
}

void soundSystem::PlayBreakSound( blocktype_t blockType, CVector pos )
{
	blockmaterial_t mat = GetBlockMaterial( blockType );
	float pitch			= 0.5 + ( random() % 15 ) / 10.0f;

	switch ( mat )
	{
		case MAT_STONE:
			m_namedSounds["breakStone"]->Play( pos, pitch, 1.0f );
			break;
		case MAT_WOOD:
			m_namedSounds["breakWood"]->Play( pos, pitch, 1.0f );
			break;
		case MAT_LOOSE:
			m_namedSounds["breakLoose"]->Play( pos, pitch, 1.0f );
			break;
		case MAT_GLASS:
			m_namedSounds["breakGlass"]->Play( pos, pitch, 1.0f );
			break;
		case MAT_ORGANIC:
			m_namedSounds["breakOrganic"]->Play( pos, pitch, 1.0f );
			break;
	}
}
void soundSystem::PlayPlaceSound( blocktype_t blockType, CVector pos )
{
	blockmaterial_t mat = GetBlockMaterial( blockType );
	float pitch			= 0.5 + ( random() % 15 ) / 10.0f;

	switch ( mat )
	{
		case MAT_STONE:
			m_namedSounds["placeStone"]->Play( pos, pitch, 1.0f );
			break;
		case MAT_WOOD:
			m_namedSounds["placeWood"]->Play( pos, pitch, 1.0f );
			break;
		case MAT_LOOSE:
			m_namedSounds["placeLoose"]->Play( pos, pitch, 1.0f );
			break;
		case MAT_GLASS:
			m_namedSounds["placeGlass"]->Play( pos, pitch, 1.0f );
			break;
		case MAT_ORGANIC:
			m_namedSounds["placeOrganic"]->Play( pos, pitch, 1.0f );
			break;
	}
}