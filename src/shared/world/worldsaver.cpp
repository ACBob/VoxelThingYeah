#include "worldsaver.hpp"

#include "shared/logging.hpp"
#include "world.hpp"

#include "chunk.hpp"

#include "../blockdef.hpp"

bool worldIO::saveWorld(CWorld *pWorld, const char *filename)
{
	// Start the file
	fileSystem::File *f = new fileSystem::File(filename, fileSystem::WRITE);
	if( !f || !f->IsOpen() )
	{
		delete f;
		con_error( "Could not open file %s for saving world", filename );
		return false;
	}

	// Write the header
	f->Write( WORLDHEADER, 4 );
	// Write the version
	f->Write( WORLDVERSION );

	// Block Palette Size (BLOCKID enum)
	f->Write( 2 * LASTBLOCK );
	
	// Write the block palette
	for( int i = 0; i < LASTBLOCK + 1; i++ )
	{
		f->Write( i );
		f->Write( i );
	}

	// Chunk count
	f->Write( pWorld->m_chunks.size() );

	// Write the chunks
	for (auto &&c : pWorld->m_chunks )
	{
		// Write the chunk position
		f->Write( c->m_vPosition.x );
		f->Write( c->m_vPosition.y );
		f->Write( c->m_vPosition.z );

		f->Write( CHUNKSIZE_X );
		f->Write( CHUNKSIZE_Y );
		f->Write( CHUNKSIZE_Z );

		// Write the blocks
		for( int x = 0; x < CHUNKSIZE_X*CHUNKSIZE_Y*CHUNKSIZE_Z; x++ )
		{
			// TODO: convert to palette index
			f->Write( c->m_blocks[x].m_iBlockType );
		}
	}

	// Write the time of day
	f->Write( pWorld->m_iTimeOfDay );

	// Close
	f->Close();

	// Delete the file
	delete f;

	return true;
}