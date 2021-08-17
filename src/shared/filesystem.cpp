#include "filesystem.h"

#include <cstdio>

#include "physfs.h"
#include "seethe.h"

#define PRINTPHYSFSERR \
		con_error("PhysFS Error: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()))

namespace fileSystem
{

	bool Init(const char* exePath)
	{
		// Initialise PhysFS first and foremost
		if (PHYSFS_init(exePath) == 0)
		{
			PRINTPHYSFSERR;
			return false;
		}

		return true;
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
			PRINTPHYSFSERR;
			success = false;
		}

		return buf;
	}

	bool Mount(const char* realPath, const char* virtualPath, bool prepend)
	{
		if (PHYSFS_mount(realPath, virtualPath, prepend ? 0 : 1) == 0)
		{
			PRINTPHYSFSERR;
			return false;
		}

		return true;
	}
}