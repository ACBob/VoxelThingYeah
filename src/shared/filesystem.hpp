#pragma once

#include <inttypes.h>

#include <vector>

// Thin wrapper around PhysFS.
namespace fileSystem
{
	// exePath is usually argv[0].
	bool Init( const char *exePath );

	// Len is set to the length of the file
	// Returns loaded content (if any) even on error
	// Success is set depending on if it succeeds or not
	const unsigned char *LoadFile( const char *virtualPath, int64_t &len, bool &success );

	// data is the bytes to write
	// dataLength is the length of the bytes
	// Success is set depending on if it succeeds or not
	void WriteFile( const char *virtualPath, const unsigned char *data, int64_t dataLength, bool &success );

	// Mounts realPath at virtualPath in PhysFS
	// Prepend will place the directory first in the search path, else at the end
	// TODO: realPath seems to be relative to the CWD??
	bool Mount( const char *realPath, const char *virtalPath, bool prepend = false );

	// Unmounts the realpath
	void UnMount( const char *realPath );

	// Unmounts every directory already mounted
	void UnMountAll();

	extern std::vector<const char*> mountedPaths;

	// Sets the path for writing.
	bool MountWrite( const char *realPath );

	// TRUE if the path exists on the virtual file system at all
	bool Exists( const char *virtualPath );

	// Lists the directory
	std::vector<const char*> List(const char* path);

	void UnInit();
} // namespace fileSystem