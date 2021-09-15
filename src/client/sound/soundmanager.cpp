#include "soundmanager.hpp"

#include "tomlcpp.hpp"

#include "blockdef.hpp"

#include <random>

#include "utility/raycast.hpp"

#ifdef __linux__
	#define AL_ALEXT_PROTOTYPES
	#include <AL/al.h>
	#include <AL/alc.h>
	#include <AL/efx-presets.h>
	#include <AL/efx.h>
#elif _WIN32
	// Mostly my setup, idk what it's like on an actual windows host
	#include <al.h>
	#include <alc.h>
#else
	#error Unsupported Platform!
#endif

#include "filesystem.hpp"

#define LOG_LEVEL DEBUG
#include "shared/seethe.h"

#include "cvar_clientside.hpp"

#include "stb_vorbis.c"

std::vector<CSound *> soundSystem::loadedSounds;
std::map<std::string, CSoundEvent *> soundSystem::soundEvents;

ALCdevice *openAlDevice;
ALCcontext *openAlContext;
ALuint soundEffects;
ALuint soundEffectSlot;
// ALuint soundFilter;

// TODO: Error sound
CSound::CSound( const char *path )
{
	bool bSuccess		= false;
	int64_t iFileLength = 0;

	const uchar_t *soundData = fileSystem::LoadFile( path, iFileLength, bSuccess );

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
#ifdef __linux__
	alSource3i( m_iId, AL_AUXILIARY_SEND_FILTER, soundEffectSlot, 0, AL_FILTER_NULL );
#endif

	alSourcePlay( m_iId );
}

CSoundEvent::CSoundEvent( std::vector<std::string> sounds, const char *type, float minpitch, float maxpitch )
{
	for ( std::string soundName : sounds )
	{
		CSound *s = soundSystem::LoadSound( soundName.c_str() );
		m_sounds.push_back( s );
	}

	if ( strcmp( type, "block" ) == 0 )
	{
		m_iSoundType = SOUNDTYPE_BLOCK;
	}

	m_fMinPitch = minpitch;
	m_fMaxPitch = maxpitch;
}

void CSoundEvent::Play( CVector pos )
{
	float pitch = m_fMinPitch + ( m_fMaxPitch - m_fMinPitch ) * ( ( rand() ) / (float)RAND_MAX );

	int soundIdx = ( rand() % m_sounds.size() );

	m_sounds.at( soundIdx )->Play( pos, pitch, pitch );
}

void soundSystem::Init()
{
	openAlDevice = alcOpenDevice( NULL );
	if ( !openAlDevice )
	{
		con_error( "OpenAL Initialisation Failed!" );
		return;
	}

#ifdef __linux__
	if ( alcIsExtensionPresent( openAlDevice, "ALC_EXT_EFX" ) == AL_FALSE )
	{
		con_warning( "Device does not support ALC_EXT_EFX, no cool reverb for you :(" );
		cl_reverb->SetBool( false );
	}
#else
	con_warning( "Reverb support currently disabled for Windows." );
#endif

	openAlContext = alcCreateContext( openAlDevice, NULL );
	if ( !openAlContext )
	{
		con_error( "OpenAL Intialisation Failed!" );
	}
	if ( !alcMakeContextCurrent( openAlContext ) )
	{
		con_error( "Could not make AL context current!" );
	}

#ifdef __linux__
	if ( cl_reverb->GetBool() )
	{
		// Effects
		alGenAuxiliaryEffectSlots( 1, &soundEffectSlot );
		alGenEffects( 1, &soundEffects );

		alEffecti( soundEffects, AL_EFFECT_TYPE, AL_EFFECT_REVERB );
		alEffectf( soundEffects, AL_REVERB_DECAY_TIME, 0.1f );

		alAuxiliaryEffectSloti( soundEffectSlot, AL_EFFECTSLOT_EFFECT, soundEffects );
	}
#endif

	alListener3f( AL_POSITION, 0.0f, 0.0f, 0.0f );
	alListener3f( AL_VELOCITY, 0.0f, 0.0f, 0.0f );
	float ori[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };
	alListenerfv( AL_ORIENTATION, ori );
	alListenerf( AL_GAIN, cl_volume->GetFloat() );

	/* Now load the sounds */
	con_info( "Loading sounds.toml" );

	int64_t fl				  = 0;
	bool bSuccess			  = false;
	const uchar_t *soundsToml = fileSystem::LoadFile( "sound/sounds.toml", fl, bSuccess );

	toml::Result data = toml::parse( (char *)soundsToml );

	for ( std::string key : data.table->keys() )
	{
		auto tbl = data.table->getTable( key );

		auto [has, type] = tbl->getString( "type" );
		if ( !has )
			// Assume type
			type = "";

		auto sounds = tbl->getArray( "sounds" );
		if ( !sounds )
			con_warning( "Sound %s has no sounds set!", key.c_str() );

		float minPitch, maxPitch;

		auto pitch = tbl->getArray( "pitch" );
		auto pA	   = pitch->getDouble( 0 );
		if ( !pA.first )
			minPitch = 1.0f;
		else
			minPitch = pA.second;
		auto pB = pitch->getDouble( 1 );
		if ( !pB.first )
			maxPitch = 1.0f;
		else
			maxPitch = pB.second;

		soundEvents[key] = new CSoundEvent( *sounds->getStringVector().get(), type.c_str(), minPitch, maxPitch );
	}
}
void soundSystem::UnInit()
{
	for ( CSound *s : loadedSounds )
		delete s;
	for ( auto s : soundEvents )
		delete s.second;

#ifdef __linux__
	alDeleteEffects( 1, &soundEffects );
	alDeleteAuxiliaryEffectSlots( 1, &soundEffects );
#endif

	// Shutdown sound
	alcDestroyContext( openAlContext );
	alcCloseDevice( openAlDevice );
}

void soundSystem::SetListener( CWorld *world, CVector pos, CVector forward, CVector vel )
{
	alListener3f( AL_POSITION, pos.x, pos.y, pos.z );
	float orient[] = { forward.x, forward.y, forward.z,

					   0,		  1,		 0 };
	alListenerfv( AL_ORIENTATION, orient );
	alListener3f( AL_VELOCITY, vel.x, vel.y, vel.z );

#ifdef __linux__

	// Now the magic reverb stuff
	CVoxRaycast cast;
	cast.m_vPosition = pos;
	cast.m_fLength	 = 16.0f;

	float decayTime = 0.0f;

	for ( int i = 0; i < 6; i++ )
	{
		cast.m_vDirection = DirectionVector[i];
		CPointedThing a	  = cast.Cast( world, false );

		if ( a.m_pBlock == nullptr )
		{
			// Add some reverb for things we can't touch, but be conservative of it
			decayTime += 0.2f;
			continue;
		}

		float reverb		= 0.0f;
		blockmaterial_t mat = GetBlockMaterial( a.m_pBlock->m_iBlockType );
		switch ( mat )
		{
			default:
				reverb = 0.2f;
				break;

			MAT_STONE:
			MAT_GLASS:
				reverb = 0.4f;
				break;

			MAT_LOOSE:
				reverb = 0.2f;
				break;

			MAT_ORGANIC:
				reverb = 0.15f;
				break;

			MAT_LIQUID:
				reverb = 0.0f;
				break;
		}

		decayTime += reverb * ( a.m_fDistance / 4.0f );
	}

	alEffectf( soundEffects, AL_REVERB_DECAY_TIME, decayTime );
	alAuxiliaryEffectSloti( soundEffectSlot, AL_EFFECTSLOT_EFFECT, soundEffects );

#endif
}

CSound *soundSystem::LoadSound( const char *path )
{
	con_info( "Load sound %s", path );
	CSound *snd = new CSound( path );
	loadedSounds.push_back( snd );

	return snd;
}

void soundSystem::PlayBreakSound( blocktype_t blockType, CVector pos )
{
	blockmaterial_t mat = GetBlockMaterial( blockType );

	switch ( mat )
	{
		default:
		case MAT_NONE:
		case MAT_STONE:
			soundEvents["block.break.stone"]->Play( pos );
			break;
		case MAT_LOOSE:
			soundEvents["block.break.loose"]->Play( pos );
			break;
		case MAT_WOOD:
			soundEvents["block.break.wood"]->Play( pos );
			break;
		case MAT_GLASS:
			soundEvents["block.break.glass"]->Play( pos );
			break;
		case MAT_ORGANIC:
			soundEvents["block.break.organic"]->Play( pos );
			break;
	}
}
void soundSystem::PlayPlaceSound( blocktype_t blockType, CVector pos )
{
	blockmaterial_t mat = GetBlockMaterial( blockType );

	switch ( mat )
	{
		default:
		case MAT_NONE:
		case MAT_STONE:
			soundEvents["block.place.stone"]->Play( pos );
			break;
		case MAT_LOOSE:
			soundEvents["block.place.loose"]->Play( pos );
			break;
		case MAT_WOOD:
			soundEvents["block.place.wood"]->Play( pos );
			break;
		case MAT_GLASS:
			soundEvents["block.place.glass"]->Play( pos );
			break;
		case MAT_ORGANIC:
			soundEvents["block.place.organic"]->Play( pos );
			break;
	}
}