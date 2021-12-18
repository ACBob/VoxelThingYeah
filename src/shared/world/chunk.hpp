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

	int getX();
	void setX( int x );
	int getY();
	void setY( int y );
	int getZ();
	void setZ( int z );

	int getSizeX();
	void setSizeX( int sizeX );
	int getSizeY();
	void setSizeY( int sizeY );
	int getSizeZ();
	void setSizeZ( int sizeZ );

	uint16_t getID( int x, int y, int z );
	uint16_t getMeta( int x, int y, int z );

	void setID( int x, int y, int z, uint16_t id );
	void setMeta( int x, int y, int z, uint16_t meta );

	void set( int x, int y, int z, uint16_t id, uint16_t meta );
	void set( int x, int y, int z, uint32_t voxel );

	uint32_t get( int x, int y, int z );
	void get( int x, int y, int z, uint16_t &id, uint16_t &meta );
	void get( int x, int y, int z, uint32_t &voxel );

	void set( int i, uint16_t id, uint16_t data );
	void set( int i, uint32_t voxel );

	uint32_t get( int i );
	uint16_t getID( int i );
	uint16_t getMeta( int i );
	void getID( int i, uint16_t &id );
	void getMeta( int i, uint16_t &meta );
	void get( int i, uint16_t &id, uint16_t &data );
	void get( int i, uint32_t &voxel );

	void rebuildModel();

	void render();

	Vector3i getPosition() { return m_pos; }
	Vector3i getSize() { return m_size; }

	void simulateLiquid();
	void calculateLighting();

	uint8_t getSkyLight( int x, int y, int z );
	uint16_t getBlockLight( int x, int y, int z );
	uint16_t getLighting( int x, int y, int z );

	void setSkyLight( int x, int y, int z, uint8_t light );
	void setBlockLight( int x, int y, int z, uint16_t light );
	void setBlockLight( int x, int y, int z, uint8_t red, uint8_t green, uint8_t blue );

	CWorld *m_world;

  private:
	uint32_t *m_voxels;
	uint16_t *m_lighting; // 4 bit red, 4 bit green, 4 bit blue, 4 bit sky light (16 bits)

	Vector3i m_pos;
	Vector3i m_size;
};
