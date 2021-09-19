#include "filesystem.hpp"

#include <cstdio>

#include "physfs.h"

#define LOG_LEVEL DEBUG
#include "seethe.h"

#include "types.hpp"

#include <algorithm>

#include <cstring>

namespace fileSystem
{

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

	const uchar_t *LoadFile( const char *virtualPath, int64_t &len, bool &success )
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

		int64_t fileLen = PHYSFS_fileLength( f );
		uchar_t *buf	= new uchar_t[fileLen + 1];
		success			= true;

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
		int32_t l	 = fileLen;
		uchar_t *out = new uchar_t[l + 1];
		std::copy( buf, buf + l, out );
		out[l] = '\0';

		PHYSFS_close( f );

		return out;
	}

	bool Mount( const char *realPath, const char *virtualPath, bool prepend )
	{
		if ( PHYSFS_mount( realPath, virtualPath, prepend ? 0 : 1 ) == 0 )
		{
			con_error( "%s->%s: %s", virtualPath, realpath, PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
			return false;
		}

		return true;
	}
} // namespace fileSystem