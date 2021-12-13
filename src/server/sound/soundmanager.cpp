#include "soundmanager.hpp"

#include "tomlcpp.hpp"

#include "shared/blockdef.hpp"

#include <random>

#include "filesystem.hpp"

#include "shared/logging.hpp"

#include "blocks/blockbase.hpp"

CNetworkServer *soundSystem::server;

void soundSystem::Init() {}
void soundSystem::UnInit() {}

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
void soundSystem::PlaySoundEvent( const char *soundEvent, Vector3f pos ) { server->PlaySoundEvent( soundEvent, pos ); }