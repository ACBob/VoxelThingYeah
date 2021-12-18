#include "chunk.hpp"

#include "world.hpp"

#include <random>
#include <math.h>
#include <memory>
#include <cstring>

CChunk::CChunk( int x, int y, int z, int sizeX, int sizeY, int sizeZ )
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;

	m_size.x = sizeX;
	m_size.y = sizeY;
	m_size.z = sizeZ;

	m_voxels = new uint32_t[m_size.x * m_size.y * m_size.z];
	memset( m_voxels, 0, m_size.x * m_size.y * m_size.z * sizeof( uint32_t ) );

	m_lighting = new uint16_t[m_size.x * m_size.y * m_size.z];
	memset( m_lighting, 0, m_size.x * m_size.y * m_size.z * sizeof( uint16_t ) );

	m_world = nullptr;
}

CChunk::~CChunk() { delete[] m_voxels; }

int CChunk::getX() { return m_pos.x; }
void CChunk::setX( int x ) { m_pos.x = x; }
int CChunk::getY() { return m_pos.y; }
void CChunk::setY( int y ) { m_pos.y = y; }
int CChunk::getZ() { return m_pos.z; }
void CChunk::setZ( int z ) { m_pos.z = z; }

int CChunk::getSizeX() { return m_size.x; }
void CChunk::setSizeX( int sizeX ) { m_size.x = sizeX; }
int CChunk::getSizeY() { return m_size.y; }
void CChunk::setSizeY( int sizeY ) { m_size.y = sizeY; }
int CChunk::getSizeZ() { return m_size.z; }
void CChunk::setSizeZ( int sizeZ ) { m_size.z = sizeZ; }

uint16_t CChunk::getID( int x, int y, int z )
{
	if ( x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z )
	{
		return 0;
	}
	return m_voxels[x + y * m_size.x + z * m_size.x * m_size.y];
}
uint16_t CChunk::getMeta( int x, int y, int z )
{
	if ( x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z )
	{
		return 0;
	}
	return m_voxels[x + y * m_size.x + z * m_size.x * m_size.y] >> 16;
}

void CChunk::setID( int x, int y, int z, uint16_t id ) { set( x, y, z, id, getMeta( x, y, z ) ); }

void CChunk::setMeta( int x, int y, int z, uint16_t meta ) { set( x, y, z, getID( x, y, z ), meta ); }

void CChunk::set( int x, int y, int z, uint16_t id, uint16_t meta )
{
	if ( x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z )
	{
		return;
	}
	m_voxels[x + y * m_size.x + z * m_size.x * m_size.y] = id | ( meta << 16 );
}

void CChunk::set( int x, int y, int z, uint32_t voxel )
{
	if ( x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z )
	{
		return;
	}
	m_voxels[x + y * m_size.x + z * m_size.x * m_size.y] = voxel;
}

uint32_t CChunk::get( int x, int y, int z )
{
	if ( x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z )
	{
		return 0;
	}
	return m_voxels[x + y * m_size.x + z * m_size.x * m_size.y];
}

void CChunk::get( int x, int y, int z, uint32_t &voxel )
{
	if ( x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z )
	{
		return;
	}
	voxel = m_voxels[x + y * m_size.x + z * m_size.x * m_size.y];
}

void CChunk::get( int x, int y, int z, uint16_t &id, uint16_t &meta )
{
	if ( x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z )
	{
		id	 = 0;
		meta = 0;
		return;
	}
	id	 = getID( x, y, z );
	meta = getMeta( x, y, z );
}

void CChunk::set( int i, uint16_t id, uint16_t meta ) { m_voxels[i] = id | ( meta << 16 ); }

uint32_t CChunk::get( int i ) { return m_voxels[i]; }
void CChunk::set( int i, uint32_t voxel ) { m_voxels[i] = voxel; }

uint16_t CChunk::getID( int i ) { return m_voxels[i] & 0xFFFF; }
uint16_t CChunk::getMeta( int i ) { return m_voxels[i] >> 16; }

void CChunk::getID( int i, uint16_t &id ) { id = m_voxels[i] & 0xFFFF; }
void CChunk::getMeta( int i, uint16_t &meta ) { meta = m_voxels[i] >> 16; }

void CChunk::get( int i, uint16_t &id, uint16_t &meta )
{
	id	 = getID( i );
	meta = getMeta( i );
}

void CChunk::get( int i, uint32_t &voxel ) { voxel = m_voxels[i]; }

#define setIfAir( x, y, z, id, meta )                                                                                  \
	if ( getID( x, y, z ) == 0 )                                                                                       \
	{                                                                                                                  \
		setID( x, y, z, id );                                                                                          \
		setMeta( x, y, z, meta );                                                                                      \
	}

void CChunk::simulateLiquid()
{
	// TODO:
}

// first 4 bits are used for skylight
// last 12 bits are RGB
// for 16 bit lighting

uint8_t CChunk::getSkyLight( int x, int y, int z )
{
	if ( x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z )
		return 15;

	return m_lighting[x + y * m_size.x + z * m_size.x * m_size.y] >> 4;
}

uint16_t CChunk::getBlockLight( int x, int y, int z )
{
	if ( x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z )
		return 15;

	return m_lighting[x + y * m_size.x + z * m_size.x * m_size.y] & 0x0F;
}

uint16_t CChunk::getLighting( int x, int y, int z )
{
	if ( x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z )
		return 0xFFFF;

	return m_lighting[x + y * m_size.x + z * m_size.x * m_size.y];
}

void CChunk::setSkyLight( int x, int y, int z, uint8_t light )
{
	if ( x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z )
		return;

	m_lighting[x + y * m_size.x + z * m_size.x * m_size.y] = ( m_lighting[x + y * m_size.x + z * m_size.x * m_size.y] & 0xF0 ) | light;
}

void CChunk::setBlockLight( int x, int y, int z, uint16_t light )
{
	if ( x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z )
		return;

	m_lighting[x + y * m_size.x + z * m_size.x * m_size.y] = ( m_lighting[x + y * m_size.x + z * m_size.x * m_size.y] & 0x0F ) | light;
}

void CChunk::setBlockLight(int x, int y, int z, uint8_t red, uint8_t green, uint8_t blue)
{
	if (x < 0 || x >= m_size.x || y < 0 || y >= m_size.y || z < 0 || z >= m_size.z)
		return;

	uint16_t light = (red << 12) | (green << 8) | (blue << 4) | (m_lighting[x + y * m_size.x + z * m_size.x * m_size.y] & 0x0F);
	m_lighting[x + y * m_size.x + z * m_size.x * m_size.y] = light;
}

void CChunk::calculateLighting() {
	// Reading material
	// https://0fps.net/2018/02/21/voxel-lighting/
	// https://web.archive.org/web/20210429192404/https://www.seedofandromeda.com/blogs/29-fast-flood-fill-lighting-in-a-blocky-voxel-game-pt-1
	// https://web.archive.org/web/20210429192404/https://www.seedofandromeda.com/blogs/30-fast-flood-fill-lighting-in-a-blocky-voxel-game-pt-2
	// https://web.archive.org/web/20181212111600/https://notch.tumblr.com/post/434902871/per-request-this-is-how-the-new-lighting-will

	// Sky light is the last 4 bits of the lighting buffer
	// Block light is the first 12 bits, each 4 bits is a channel
	// So RGB is 12 bits each, and the last 4 bits are sky light
	// Essentially RGBA, where A is sky light

	memset( m_lighting, 0, m_size.x * m_size.y * m_size.z * sizeof( uint16_t ) );

	// Step 1. Sunlight propagation
	// test the chunk above us
	CChunk *chunkAbove = m_world->getChunk( m_pos + Vector3i( 0, 1, 0 ) );
	uint8_t *skyLight = new uint8_t[m_size.x * m_size.z];
	if (chunkAbove != nullptr) {
		for (int z = 0; z < m_size.z; z++) {
			for (int x = 0; x < m_size.x; x++) {
				skyLight[z * m_size.x + x] = chunkAbove->getSkyLight(x, 0, z);
			}
		}
	}
	else {
		// if we're above 0, we can guess there'll be skylight
		if (m_pos.y + 1 > 0) {
			for (int z = 0; z < m_size.z; z++) {
				for (int x = 0; x < m_size.x; x++) {
					skyLight[z * m_size.x + x] = 15;
				}
			}
		}
		else {
			for (int z = 0; z < m_size.z; z++) {
				for (int x = 0; x < m_size.x; x++) {
					skyLight[z * m_size.x + x] = 0;
				}
			}
		}
	}

	// Now propogate that sunlight into us
	for (int z = 0; z < m_size.z; z++) {
		for (int x = 0; x < m_size.x; x++) {
			uint8_t sky = skyLight[z * m_size.x + x];
			if (sky > 0) {
				// loop down to the bottom of the chunk, stopping if we hit a block
				for (int y = m_size.y - 1; y >= 0; y--) {
					uint16_t id = getID(x, y, z);
					if (id != 0) {
						break;
					}
					else {
						setSkyLight(x, y, z, sky);
					}
				}
			}
		}
	}
}