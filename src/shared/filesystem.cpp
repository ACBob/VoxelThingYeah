#include "filesystem.h"

#include <cstdio>

#include "physfs.h"

namespace fileSystem
{
	int Init(const char* exePath)
	{
		// Initialise PhysFS first and foremost
		if (!PHYSFS_init(exePath))
		{
			printf("PHYSFS Error!\n%s\n", PHYSFS_getLastError());
			return 1;
		}

		return 0;
	}

	void UnInit()
	{
		PHYSFS_deinit();
	}

	const unsigned char* LoadFile(const char* virtualPath, unsigned int &len, bool &success)
	{
		PHYSFS_File *f = PHYSFS_openRead(virtualPath);
		len = PHYSFS_fileLength(f);
		unsigned char buf[len];

		unsigned int rl = PHYSFS_readBytes(f, &buf, len);

		success = true;

		if (rl != len)
		{
			printf("PHYSFS File Error!\n%s\n", PHYSFS_getLastError());
			success = false;
		}

		return buf;
	}
}