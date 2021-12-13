#include "worldsaver.hpp"

#include "shared/logging.hpp"
#include "world.hpp"

#include "chunk.hpp"

#include "../shared/blockdef.hpp"

#include <cstring>

#include <algorithm>

bool worldIO::saveWorld( CWorld *pWorld, const char *filename )
{
	// Start the file
	fileSystem::File *f = new fileSystem::File( filename, fileSystem::WRITE );
	if ( !f || !f->IsOpen() )
	{
		delete f;
		con_error( "Could not open file %s for saving world", filename );
		return false;
	}

	// Write the header
	f->Write( WORLDHEADER, 4 );
	f->Write( (uint8_t)WORLDVERSION ); // Version]
	f->Write( "MeegreefWorldMeegreefWorldMeegr", 32 );

	// Write the block palette
	f->Write( (uint8_t)LASTBLOCK );
	for ( int i = 0; i < LASTBLOCK; i++ )
	{
		f->Write( (uint16_t)i );
		f->Write( (uint16_t)i );
	}

	// Write the chunk count
	f->Write( (uint32_t)pWorld->m_chunks.size() );

	// Write the chunks
	for ( auto &&chunk : pWorld->m_chunks )
	{
		PortableChunkRepresentation c = chunk->m_portableDef;

		f->Write( (uint32_t)c.x );
		f->Write( (uint32_t)c.y );
		f->Write( (uint32_t)c.z );

		// Quickly test if the chunk is just air (in which case we need to write the empty chunk header)
		// SO 14120346
		if ( std::all_of( chunk->m_blocks, chunk->m_blocks + CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z,
						  []( block_t b ) { return b.GetType() == AIR; } ) )
		{
			f->Write( MAGIC_EMPTYCHUNK, 5 );
			continue;
		}

		for ( int i = 0; i < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; i++ )
		{
			f->Write( (uint16_t)c.m_iBlocks[i] );
			f->Write( (uint16_t)c.m_iValue[i] );
		}
	}

	// Write the time of day
	f->Write( (uint16_t)pWorld->m_iTimeOfDay );

	delete f;
	return true;
}

bool worldIO::loadWorld( CWorld *world, const char *filename )
{
	// Start the file
	fileSystem::File *f = new fileSystem::File( filename, fileSystem::READ );
	if ( !f || !f->IsOpen() )
	{
		delete f;
		con_error( "Could not open file %s for loading world", filename );
		return false;
	}

	// Read the header
	char header[4];
	f->Read( header, 4 );
	if ( strncmp( header, WORLDHEADER, 4 ) != 0 )
	{
		con_error( "Invalid file format for world file %s", filename );
		delete f;
		return false;
	}

	// Read the version
	uint8_t version;
	f->Read( &version, 1 );
	if ( version != WORLDVERSION )
	{
		con_error( "Invalid version for world file %s (expected %d, got %d)", filename, WORLDVERSION, version );
		delete f;
		return false;
	}

	// Read the world name
	char worldName[32];
	f->Read( worldName, 32 );
	worldName[32] = '\0';
	con_debug( "Loading world %s", worldName );

	// Read the block palette
	uint8_t paletteSize;
	f->Read( &paletteSize, 1 );
	if ( paletteSize != LASTBLOCK )
	{
		con_error( "Invalid block palette size for world file %s (expected %d, got %d)", filename, LASTBLOCK,
				   paletteSize );
		delete f;
		return false;
	}

	// Read the block palette
	for ( int i = 0; i < LASTBLOCK; i++ )
	{
		uint16_t id, mappedId;
		f->Read( &id, 2 );
		f->Read( &mappedId, 2 );
	}

	// Read the chunk count
	uint32_t chunkCount;
	f->Read( &chunkCount, 4 );

	// Read the chunks
	for ( int i = 0; i < chunkCount; i++ )
	{
		uint32_t x, y, z;
		f->Read( &x, 4 );
		f->Read( &y, 4 );
		f->Read( &z, 4 );

		PortableChunkRepresentation c;
		c.x = x;
		c.y = y;
		c.z = z;

		// Test if the chunk is just air (in which case we need to skip it)
		// Which is flagged by MAGIC_EMPTYCHUNK
		char magic[5];
		f->Read( magic, 5 );
		if ( strncmp( magic, MAGIC_EMPTYCHUNK, 5 ) == 0 )
		{
			for ( int j = 0; j < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; j++ )
			{
				c.m_iBlocks[j] = AIR;
				c.m_iValue[j]  = 0;
			}
			continue;
		}
		// Otherwise, read the blocks
		// the first 5 bytes have been read already
		// So we need to put them back
		f->Seek( -5, fileSystem::CURRENT );

		for ( int j = 0; j < CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z; j++ )
		{
			uint16_t id;
			f->Read( &id, 2 );
			uint16_t value;
			f->Read( &value, 2 );
			c.m_iBlocks[j] = id;
			c.m_iValue[j]  = value;
		}

		world->UsePortable( c );
	}

	// Read the time of day
	uint16_t timeOfDay;
	f->Read( &timeOfDay, 2 );
	world->m_iTimeOfDay = timeOfDay;

	delete f;
	return true;
}