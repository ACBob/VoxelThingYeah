#pragma once

// Thin wrapper around PhysFS.
namespace fileSystem
{
	// exePath is usually argv[0].
	bool Init(const char* exePath);

	// Len is set to the length of the file
	// Returns loaded content (if any) even on error
	// Success is set depending on if it succeeds or not
	const unsigned char* LoadFile(const char* virtualPath, unsigned int &len, bool &success);

	void UnInit();
}