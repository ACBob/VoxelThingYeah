#include "soundmanager.hpp"

#include "tomlcpp.hpp"

#include "blockdef.hpp"

#include <random>

#include "filesystem.hpp"

#include "shared/logging.hpp"

#include "blocks/blockbase.hpp"

CNetworkServer *soundSystem::server;

void soundSystem::Init()
{
}
void soundSystem::UnInit()
{
}

void soundSystem::PlayBreakSound( BLOCKID blockType, CVector pos )
{
	BLOCKMATERIAL mat = BlockType( blockType ).GetMaterial();

	char *buf = new char[512];
	snprintf( buf, 512, "block.break.%s", BlockMaterialSTR( mat ) );
	PlaySoundEvent( buf, pos );

	delete buf;
}
void soundSystem::PlayPlaceSound( BLOCKID blockType, CVector pos )
{
	BLOCKMATERIAL mat = BlockType( blockType ).GetMaterial();

	char *buf = new char[512];
	snprintf( buf, 512, "block.place.%s", BlockMaterialSTR( mat ) );
	PlaySoundEvent( buf, pos );

	delete buf;
}
void soundSystem::PlayStepSound( BLOCKID blockType, CVector pos )
{
	BLOCKMATERIAL mat = BlockType( blockType ).GetMaterial();

	char *buf = new char[512];
	snprintf( buf, 512, "block.step.%s", BlockMaterialSTR( mat ) );
	PlaySoundEvent( buf, pos );
	delete buf;
}
void soundSystem::PlaySoundEvent( const char *soundEvent, CVector pos )
{
	server->PlaySoundEvent( soundEvent, pos );
}