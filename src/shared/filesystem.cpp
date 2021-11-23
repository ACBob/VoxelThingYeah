#include "filesystem.hpp"

#include <cstdio>

#include "physfs.h"

#include "shared/logging.hpp"

#include <algorithm>

#include <cstring>

namespace fileSystem
{
	std::vector<const char *> mountedPaths;

	bool Init( const char *exePath )
	{
		// Initialise PhysFS first and foremost
		if ( PHYSFS_init( exePath ) == 0 )
		{
			con_error( "PhysFS Error: %s", PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			return false;
		}

		return true;
	}

	void UnInit() { PHYSFS_deinit(); }

	const unsigned char *LoadFile( const char *virtualPath, int64_t &len, bool &success )
	{
		PHYSFS_File *f = PHYSFS_openRead( virtualPath );

		if ( !f )
		{
			con_error( "%s: %s", virtualPath, PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			PHYSFS_close( f );
			len		= 0;
			success = false;
			return nullptr;
		}

		int64_t fileLen	   = PHYSFS_fileLength( f );
		unsigned char *buf = new unsigned char[fileLen + 1];
		success			   = true;

		if ( PHYSFS_readBytes( f, buf, fileLen ) < fileLen )
		{
			con_error( "%s: %s", virtualPath, PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			PHYSFS_close( f );
			len		= 0;
			success = false;
			return nullptr;
		}

		buf[fileLen] = '\0';
		len			 = fileLen;

		//! FIXME
		int32_t l		   = fileLen;
		unsigned char *out = new unsigned char[l + 1];
		std::copy( buf, buf + l, out );
		out[l] = '\0';

		PHYSFS_close( f );

		return out;
	}

	void WriteFile( const char *virtualPath, const unsigned char *data, int64_t dataLength, bool &success )
	{
		PHYSFS_File *f = PHYSFS_openWrite( virtualPath );

		if ( !f )
		{
			con_error( "%s: %s", virtualPath, PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			PHYSFS_close( f );
			success = false;
			return;
		}

		if ( PHYSFS_writeBytes( f, data, dataLength ) < dataLength )
		{
			con_error( "%s: %s", virtualPath, PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			PHYSFS_close( f );
			success = false;
			return;
		}

		PHYSFS_close( f );

		success = true;
	}

	bool Mount( const char *realPath, const char *virtualPath, bool prepend )
	{
		if ( PHYSFS_mount( realPath, virtualPath, prepend ? 0 : 1 ) == 0 )
		{
			con_error( "%s->%s: %s", virtualPath, realPath, PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			return false;
		}

		char *cachepath = new char[strlen( realPath ) + 1];
		strcpy( cachepath, realPath );
		mountedPaths.push_back( cachepath );

		return true;
	}

	void UnMount( const char *realPath )
	{
		if ( PHYSFS_unmount( realPath ) == 0 )
		{
			con_error( "unmount %s: %s", realPath, PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
		}

		mountedPaths.erase( std::remove_if( mountedPaths.begin(), mountedPaths.end(),
											[realPath]( auto &c ) { return strcmp( realPath, c ) == 0; } ),
							mountedPaths.end() );
	}

	void UnMountAll()
	{
		while ( mountedPaths.size() > 0 )
			UnMount( mountedPaths.front() );
	}

	bool MountWrite( const char *realPath )
	{
		if ( PHYSFS_setWriteDir( realPath ) == 0 )
		{
			con_error( "%s->Write Path: %s", realPath, PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			return false;
		}

		return true;
	}

	bool Exists( const char *virtualPath ) { return PHYSFS_exists( virtualPath ); }

	std::vector<const char *> List( const char *path )
	{
		char **p = PHYSFS_enumerateFiles( path );
		std::vector<const char *> l;

		int i = 0;
		while ( p[i] != NULL )
		{
			char *c = new char[strlen( p[i] ) + 1]();
			strcpy( c, p[i] );

			l.push_back( c );

			i++;
		}

		PHYSFS_freeList( p );

		return l;
	}


	File::File( const char *virtualPath, FILEMODE mode )
	{
		if ( mode == FILEMODE::READ )
		{
			m_pHandle = PHYSFS_openRead( virtualPath );

			m_size = PHYSFS_fileLength( (PHYSFS_File*)m_pHandle );
		}
		else if ( mode == FILEMODE::WRITE )
		{
			m_pHandle = PHYSFS_openWrite( virtualPath );

			m_size = 0;
		}

		if ( !m_pHandle )
		{
			con_error( "Could not open %s: %s", virtualPath, PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			m_size = 0;
		}

		m_bOpened = true;
	}

	File::~File()
	{
		if ( m_bOpened )
		{
			PHYSFS_close( (PHYSFS_File*)m_pHandle );
		}
	}

	int64_t File::Read( void *buffer, int64_t len )
	{
		if ( !m_bOpened )
			return 0;

		int64_t r = PHYSFS_readBytes( (PHYSFS_File*)m_pHandle, buffer, len );

		if ( r < 0 )
		{
			con_error( "Read error: %s", PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			return 0;
		}

		m_position = PHYSFS_tell( (PHYSFS_File*)m_pHandle );

		return r;
	}

	int64_t File::Read( int &i, int64_t len )
	{
		if ( !m_bOpened )
			return 0;
			
		int64_t r = PHYSFS_readBytes( (PHYSFS_File*)m_pHandle, &i, len );

		if ( r < 0 )
		{
			con_error( "Read error: %s", PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			return 0;
		}

		m_position = PHYSFS_tell( (PHYSFS_File*)m_pHandle );

		return r;
	}

	int64_t File::Read( int &i )
	{
		return Read( &i, sizeof( i ) );
	}

	int64_t File::Write( const void *buffer, int64_t len )
	{
		if ( !m_bOpened )
			return 0;

		int64_t r = PHYSFS_writeBytes( (PHYSFS_File*)m_pHandle, buffer, len );

		if ( r < 0 )
		{
			con_error( "Write error: %s", PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			return 0;
		}

		m_position = PHYSFS_tell( (PHYSFS_File*)m_pHandle );

		return r;
	}

	int64_t File::Write( int value )
	{
		return Write( &value, sizeof( value ) );
	}

	int64_t File::Write( uint64_t value )
	{
		return Write( &value, 8 );
	}
	int64_t File::Write( uint32_t value )
	{
		return Write( &value, 4 );
	}
	int64_t File::Write( uint16_t value )
	{
		return Write( &value, 2 );
	}
	int64_t File::Write( uint8_t value )
	{
		return Write( &value, 1 );
	}

	int64_t File::Seek( int64_t pos, SEEKMODE type )
	{
		if ( !m_bOpened )
			return 0;

		if (type == CURRENT)
			pos += m_position;
		else if (type == END)
			pos += m_size;

		int64_t r = PHYSFS_seek( (PHYSFS_File*)m_pHandle, pos );

		m_position = PHYSFS_tell( (PHYSFS_File*)m_pHandle );

		if ( r < 0 )
		{
			con_error( "Seek error: %s", PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			return 0;
		}

		return r;
	}

	int64_t File::Tell()
	{
		if ( !m_bOpened )
			return 0;

		int64_t r = PHYSFS_tell( (PHYSFS_File*)m_pHandle );

		if ( r < 0 )
		{
			con_error( "Tell error: %s", PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			return 0;
		}

		return r;
	}

	int64_t File::Size()
	{
		return m_size;
	}

	bool File::IsOpen()
	{
		return m_bOpened;
	}

	void File::Close()
	{
		if ( m_bOpened )
		{
			PHYSFS_close( (PHYSFS_File*)m_pHandle );
			m_bOpened = false;
		}
	}

	bool File::Eof()
	{
		if ( !m_bOpened )
			return true;

		return PHYSFS_eof( (PHYSFS_File*)m_pHandle );
	}

	bool File::Flush()
	{
		if ( !m_bOpened )
			return false;

		return PHYSFS_flush( (PHYSFS_File*)m_pHandle ) != 0;
	}
	
	const char *File::GetVirtualPath() { return m_virtualPath; }
} // namespace fileSystem