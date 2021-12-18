// -- Chunk

// Concepts:
// "Block" - uint16_t Block ID.
// "Meta" - uint16_t Block Meta.
// "Voxel" - uint32_t Block ID + Meta.
// "Chunk" - Collection of Blocks.

// Default chunk size
// Used if the map doesn't specify, or on new maps
#define DEF_CHUNK_SIZE_X 16
#define DEF_CHUNK_SIZE_Y 16
#define DEF_CHUNK_SIZE_Z 16

class CWorld;	   // Forward declaration

#include "utility/vector.hpp"

#include <inttypes.h>

struct PortableChunkRepresentation; // TODO:

class CChunk
{
  public:
	CChunk( int x, int y, int z, int sizeX = DEF_CHUNK_SIZE_X, int sizeY = DEF_CHUNK_SIZE_Y,
			int sizeZ = DEF_CHUNK_SIZE_Z );
	~CChunk();

	virtual int getX();
	virtual void setX( int x );
	virtual int getY();
	virtual void setY( int y );
	virtual int getZ();
	virtual void setZ( int z );

	virtual int getSizeX();
	virtual void setSizeX( int sizeX );
	virtual int getSizeY();
	virtual void setSizeY( int sizeY );
	virtual int getSizeZ();
	virtual void setSizeZ( int sizeZ );

	virtual uint16_t getID( int x, int y, int z );
	virtual uint16_t getMeta( int x, int y, int z );

	virtual void setID( int x, int y, int z, uint16_t id );
	virtual void setMeta( int x, int y, int z, uint16_t meta );

	virtual void set( int x, int y, int z, uint16_t id, uint16_t meta );
	virtual void set( int x, int y, int z, uint32_t voxel );

	virtual uint32_t get( int x, int y, int z );
	virtual void get( int x, int y, int z, uint16_t &id, uint16_t &meta );
	virtual void get( int x, int y, int z, uint32_t &voxel );

	virtual void set( int i, uint16_t id, uint16_t data );
	virtual void set( int i, uint32_t voxel );

	virtual uint32_t get( int i );
	virtual uint16_t getID( int i );
	virtual uint16_t getMeta( int i );
	virtual void getID( int i, uint16_t &id );
	virtual void getMeta( int i, uint16_t &meta );
	virtual void get( int i, uint16_t &id, uint16_t &data );
	virtual void get( int i, uint32_t &voxel );

	Vector3i getPosition() { return m_pos; }
	Vector3i getSize() { return m_size; }

	virtual void simulateLiquid();
	virtual void calculateLighting();

	uint8_t getSkyLight( int x, int y, int z );
	uint16_t getBlockLight( int x, int y, int z );
	uint16_t getLighting( int x, int y, int z );

	virtual void setSkyLight( int x, int y, int z, uint8_t light );
	virtual void setBlockLight( int x, int y, int z, uint16_t light );
	virtual void setBlockLight( int x, int y, int z, uint8_t red, uint8_t green, uint8_t blue );

	CWorld *m_world;

  protected:
	uint32_t *m_voxels;
	uint16_t *m_lighting; // 4 bit red, 4 bit green, 4 bit blue, 4 bit sky light (16 bits)

	Vector3i m_pos;
	Vector3i m_size;
};
