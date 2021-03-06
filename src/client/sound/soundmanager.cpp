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

#include "shared/logging.hpp"

#include "cvar_clientside.hpp"

#include "stb_vorbis.c"

#include "blocks/blockbase.hpp"

std::map<std::string, CSound *> soundSystem::loadedSounds;
std::map<std::string, CSoundEvent *> soundSystem::soundEvents;

ALCdevice *openAlDevice;
ALCcontext *openAlContext;
ALuint soundEffects;
ALuint soundEffectSlot;
ALuint soundMuffle;
ALuint soundMuffleSlot;

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

	uint32_t l = decode_len * channels * ( sizeof( int16_t ) / sizeof( uint8_t ) );
	alGenBuffers( 1, &m_iBuffer );
	alBufferData( m_iBuffer, channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, data, l, rate );

	// Generate a few sources for the sound
	for ( int i = 0; i < SOURCES_PER_SOUND; i++ )
	{
		alGenSources( 1, &m_iSources[i] );
		alSourcef( m_iSources[i], AL_PITCH, 1.0 );
		alSourcef( m_iSources[i], AL_GAIN, 1.0 );
		alSource3f( m_iSources[i], AL_POSITION, 0, 0, 0 );
		alSource3f( m_iSources[i], AL_VELOCITY, 0, 0, 0 );
		alSourcei( m_iSources[i], AL_SOURCE_TYPE, AL_STATIC );
		alSourcei( m_iSources[i], AL_LOOPING, 0 );

		alSourcei( m_iSources[i], AL_BUFFER, m_iBuffer );
	}

	delete[] soundData;
}
void CSound::Play( Vector3f src, float pitch, float gain )
{
	// Select the first available source
	// A source is available if it is not playing
	// or if it is playing but has stopped
	// TODO: Should we have a global list of sources?
	unsigned int iSource = -1;
	for ( int i = 0; i < SOURCES_PER_SOUND; i++ )
	{
		ALint state;
		alGetSourcei( m_iSources[i], AL_SOURCE_STATE, &state );

		if ( state == AL_PLAYING )
		{
			continue;
		}

		iSource = m_iSources[i];
		break;
	}

	if ( iSource == -1 )
	{
		// Forceably steal a source at random
		unsigned int stealSource = rand() % SOURCES_PER_SOUND;
		alSourceStop( m_iSources[stealSource] );
		iSource = m_iSources[stealSource];
	}

	alSourcef( iSource, AL_PITCH, pitch );
	alSourcef( iSource, AL_GAIN, gain );
	alSource3f( iSource, AL_POSITION, src.x, src.y, src.z );
#ifdef __linux__
	// Apply Reverb & Muffle
	alSourcei( iSource, AL_DIRECT_FILTER, soundEffects );
	alSourcei( iSource, AL_AUXILIARY_SEND_FILTER, soundMuffle );
#endif

	alSourcePlay( iSource );
}

CSoundEvent::CSoundEvent( std::vector<std::string> sounds, const char *type, float minpitch, float maxpitch )
{
	for ( std::string soundName : sounds )
	{
		CSound *s = soundSystem::LoadSound( ( "/assets/sounds/" + soundName ).c_str() );
		m_sounds.push_back( s );
	}

	// TODO: figure out better

	if ( strcmp( type, "block" ) == 0 )
	{
		m_iSoundType = SOUNDTYPE_BLOCK;
	}
	else if ( strcmp( type, "music" ) == 0 )
	{
		m_iSoundType = SOUNDTYPE_MUSIC;
	}
	else if ( strcmp( type, "ui" ) == 0 )
	{
		m_iSoundType = SOUNDTYPE_GUI;
	}
	else if ( strcmp( type, "entity" ) == 0 )
	{
		m_iSoundType = SOUNDTYPE_ENTITY;
	}

	m_fMinPitch = minpitch;
	m_fMaxPitch = maxpitch;
}

void CSoundEvent::Play( Vector3f pos )
{
	if ( m_sounds.size() <= 0 )
		return;

	float pitch = m_fMinPitch + ( m_fMaxPitch - m_fMinPitch ) * ( ( rand() ) / (float)RAND_MAX );

	int soundIdx = 0;
	if ( m_sounds.size() >= 2 )
		soundIdx = ( rand() % m_sounds.size() );

	// TODO: Seperate volume variables
	float gain = 1.0;
	switch ( m_iSoundType )
	{
		case SOUNDTYPE_BLOCK:
			gain = 1.0f;
			break;
		case SOUNDTYPE_MUSIC:
			gain = 0.25f;
			break;
		case SOUNDTYPE_GUI:
			gain = 1.0f;
			break;
		case SOUNDTYPE_ENTITY:
			gain = 1.0f;
			break;
	}

	m_sounds.at( soundIdx )->Play( pos, pitch, gain );
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

	// For muffling sounds
	if ( cl_mufflesounds->GetBool() )
	{
		alGenFilters( 1, &soundMuffle );
		alFilteri( soundMuffle, AL_FILTER_TYPE, AL_FILTER_LOWPASS );
		alFilterf( soundMuffle, AL_LOWPASS_GAIN, 1.0f );
		alFilterf( soundMuffle, AL_LOWPASS_GAINHF, 1.0f );
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
	const uchar_t *soundsToml = fileSystem::LoadFile( "/assets/sounds/sounds.toml", fl, bSuccess );

	toml::Result data = toml::parse( bSuccess ? (char *)soundsToml : "\0" );

	for ( std::string key : data.table->keys() )
	{
		auto tbl = data.table->getTable( key );
		// con_debug( "Found sound event: %s", key.c_str() );

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

	delete[] soundsToml;
}
void soundSystem::UnInit()
{
	for ( auto s : loadedSounds )
		delete s.second;
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

void soundSystem::SetListener( CWorld *world, Vector3f pos, Vector3f forward, Vector3f vel )
{
	alListener3f( AL_POSITION, pos.x, pos.y, pos.z );
	float orient[] = { forward.x, forward.y, forward.z,

					   0,		  1,		 0 };
	alListenerfv( AL_ORIENTATION, orient );
	alListener3f( AL_VELOCITY, vel.x, vel.y, vel.z );
	alListenerf( AL_GAIN, cl_volume->GetFloat() );

#ifdef __linux__

	if ( cl_reverb->GetBool() && world != nullptr )
	{
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

			float reverb	  = 0.0f;
			BLOCKMATERIAL mat = BlockType( a.m_pBlock->GetType() ).GetMaterial( a.m_pBlock->GetMeta() );
			switch ( mat )
			{
				default:
					reverb = 0.05f;
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
	}
	else if ( world == nullptr )
	{
		alEffectf( soundEffects, AL_REVERB_DECAY_TIME, 0.0f );
		alAuxiliaryEffectSloti( soundEffectSlot, AL_EFFECTSLOT_EFFECT, soundEffects );
	}

	if ( cl_mufflesounds->GetBool() && world != nullptr )
	{
		// If the listener is inside liquid, we need to muffle the sounds
		// we can just get the position of the listener

		block_t *block = world->BlockAtWorldPos( pos );
		if ( block != nullptr && ( block->GetType() == WATER || block->GetType() == WATERSRC ) )
		{
			alFilterf( soundMuffle, AL_LOWPASS_GAIN, 0.2f );
			alFilterf( soundMuffle, AL_LOWPASS_GAINHF, 0.2f );

			// Also change the speed of sound
			// (this is subtle, but it's neat)
			alSpeedOfSound( 1493.0f );
		}
		else
		{
			alFilterf( soundMuffle, AL_LOWPASS_GAIN, 1.0f );
			alFilterf( soundMuffle, AL_LOWPASS_GAINHF, 1.0f );

			// Also change the speed of sound
			alSpeedOfSound( 343.3f );
		}
	}
	else if ( world == nullptr )
	{
		alFilterf( soundMuffle, AL_LOWPASS_GAIN, 1.0f );
		alFilterf( soundMuffle, AL_LOWPASS_GAINHF, 1.0f );

		// Also change the speed of sound
		alSpeedOfSound( 343.3f );
	}
#endif
}

CSound *soundSystem::LoadSound( const char *path )
{
	if ( loadedSounds.count( path ) )
		return loadedSounds[path];

	CSound *snd		   = new CSound( path );
	loadedSounds[path] = snd;

	return snd;
}

void soundSystem::PlayBreakSound( BLOCKID blockType, Vector3f pos )
{
	BLOCKMATERIAL mat = BlockType( blockType ).GetMaterial( 0 );

	char *buf = new char[512];
	snprintf( buf, 512, "block.break.%s", BlockMaterialSTR( mat ) );
	PlaySoundEvent( buf, pos );

	delete buf;
}
void soundSystem::PlayPlaceSound( BLOCKID blockType, Vector3f pos )
{
	BLOCKMATERIAL mat = BlockType( blockType ).GetMaterial( 0 );

	char *buf = new char[512];
	snprintf( buf, 512, "block.place.%s", BlockMaterialSTR( mat ) );
	PlaySoundEvent( buf, pos );

	delete buf;
}
void soundSystem::PlayStepSound( BLOCKID blockType, Vector3f pos )
{
	BLOCKMATERIAL mat = BlockType( blockType ).GetMaterial( 0 );

	char *buf = new char[512];
	snprintf( buf, 512, "block.step.%s", BlockMaterialSTR( mat ) );
	PlaySoundEvent( buf, pos );
	delete buf;
}
void soundSystem::PlaySoundEvent( CSoundEvent *event, Vector3f pos ) { event->Play( pos ); }
void soundSystem::PlaySoundEvent( const char *soundEvent, Vector3f pos )
{
	if ( !soundEvents.count( soundEvent ) )
	{
		con_error( "Unknown Sound Event %s", soundEvent );
		return;
	}

	PlaySoundEvent( soundEvents[soundEvent], pos );
}