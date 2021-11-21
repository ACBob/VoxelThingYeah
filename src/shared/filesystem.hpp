#pragma once

#include <inttypes.h>

#include <vector>

// Thin wrapper around PhysFS.
namespace fileSystem
{
	class File;

	enum FILEMODE
	{
		READ,
		WRITE
	};

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

	// Opens the file
	File *OpenFile( const char *virtualPath, const char *mode );

	// Mounts realPath at virtualPath in PhysFS
	// Prepend will place the directory first in the search path, else at the end
	// TODO: realPath seems to be relative to the CWD??
	bool Mount( const char *realPath, const char *virtalPath, bool prepend = false );

	// Unmounts the realpath
	void UnMount( const char *realPath );

	// Unmounts every directory already mounted
	void UnMountAll();

	extern std::vector<const char *> mountedPaths;

	// Sets the path for writing.
	bool MountWrite( const char *realPath );

	// TRUE if the path exists on the virtual file system at all
	bool Exists( const char *virtualPath );

	// Lists the directory
	std::vector<const char *> List( const char *path );

	void UnInit();

	class File
	{
	  public:
		File( const char *virtualPath, FILEMODE mode );
		~File();

		// Returns the number of bytes read
		int64_t Read( void *buffer, int64_t len );

		// Returns the number of bytes read
		int64_t Read( int &i, int64_t len );

		// Returns the number of bytes read
		int64_t Read( int &i );

		// Returns the number of bytes written
		int64_t Write( const void *buffer, int64_t len );

		// Returns the number of bytes written
		int64_t Write( int value );

		// Returns the number of bytes written
		int64_t Write( uint64_t value );
		int64_t Write( uint32_t value );
		int64_t Write( uint16_t value );
		int64_t Write( uint8_t value );

		// Returns the number of bytes read
		int64_t Seek( int64_t offset, int to );

		// Returns the current position
		int64_t Tell();

		// Returns the size of the file
		int64_t Size();

		// Returns true if the file is open
		bool IsOpen();

		// Closes the file
		void Close();

		// Returns true if the file is at the end
		bool Eof();

		// Flushes the file
		bool Flush();

		// Returns the virtual path
		const char *GetVirtualPath();

		private:
		bool m_bOpened;
		FILEMODE m_mode;
		const char *m_virtualPath;
		int64_t m_position;
		int64_t m_size;
		void *m_pHandle;
	};
} // namespace fileSystem