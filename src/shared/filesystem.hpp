#pragma once

#include "types.hpp"

// Thin wrapper around PhysFS.
namespace fileSystem
{
	// exePath is usually argv[0].
	bool Init( const char *exePath );

	// Len is set to the length of the file
	// Returns loaded content (if any) even on error
	// Success is set depending on if it succeeds or not
	const unsigned char *LoadFile( const char *virtualPath, int64_t &len, bool &success );

	// Mounts realPath at virtualPath in PhysFS
	// Prepend will place the directory first in the search path, else at the end
	// TODO: realPath seems to be relative to the CWD??
	bool Mount( const char *realPath, const char *virtalPath, bool prepend = false );

	void UnInit();
} // namespace fileSystem