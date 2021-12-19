#include "raycast.hpp"

#include "chunk.hpp"
#include "world.hpp"

#include <math.h>

CRaycast::CRaycast()
{
	m_start		= Vector3f( 0, 0, 0 );
	m_fLength	= 5.0f;
	m_direction = Vector3f( 0, 0, 1 );
}

CRaycast::~CRaycast() {}

// std::pair<Vector3f, Vector3f> CRaycast::cast( CChunk *chunk )
// {
// 	Vector3f ray = m_start;
// 	Vector3f oRay; // Used for figuring out the normal

// 	const float step = 0.01f;
// 	float i			 = 0.0f;

// 	while ( i < m_fLength )
// 	{
// 		oRay = ray;
// 		ray	 = m_start + m_direction * i;
// 		i += step;

// 		int x = floor( ray.x() );
// 		int y = floor( ray.y() );
// 		int z = floor( ray.z() );

// 		if ( chunk->getID( x, y, z ) > 0 )
// 		{
// 			Vector3f normal =
// 				Vector3f( floor( oRay.x() ), floor( oRay.y() ), floor( oRay.z() ) ) - Vector3f( x, y, z );
// 			normal.normalize();
// 			return { ray, normal };
// 		}
// 	}

// 	return { Vector3f( 0, 0, 0 ), Vector3f( 0, 0, 0 ) };
// }

// std::pair<Vector3f, Vector3f> CRaycast::cast( CWorld *world, Vector3f start, Vector3f direction, float length )
// {
// 	m_start		= start;
// 	m_direction = direction;
// 	m_fLength	= length;

// 	return cast( world );
// }

std::pair<Vector3f, Vector3f> CRaycast::cast( CWorld *world )
{
	Vector3f ray = m_start;
	Vector3f oRay; // Used for figuring out the normal

	const float step = 0.01f;
	float i			 = 0.0f;

	while ( i < m_fLength )
	{
		oRay = ray;
		ray	 = m_start + m_direction * i;
		i += step;

		int x = floor( ray.x );
		int y = floor( ray.y );
		int z = floor( ray.z );

		if ( world->getID( x, y, z ) > 0 )
		{
			Vector3f normal = Vector3f( floor( oRay.x ), floor( oRay.y ), floor( oRay.z ) ) - Vector3f( x, y, z );
			normal.Normal();
			return { ray, normal };
		}
	}

	return { Vector3f( 0, 0, 0 ), Vector3f( 0, 0, 0 ) };
}

std::pair<Vector3f, Vector3f> CRaycast::cast( CWorld *world, Vector3f start, Vector3f direction, float length )
{
	Vector3f ray = start;
	Vector3f oRay; // Used for figuring out the normal

	const float step = 0.01f;
	float i			 = 0.0f;

	while ( i < length )
	{
		oRay = ray;
		ray	 = start + direction * i;
		i += step;

		int x = floor( ray.x );
		int y = floor( ray.y );
		int z = floor( ray.z );

		if ( world->getID( x, y, z ) > 0 )
		{
			Vector3f normal = Vector3f( floor( oRay.x ), floor( oRay.y ), floor( oRay.z ) ) - Vector3f( x, y, z );
			normal.Normal();
			return { ray, normal };
		}
	}

	return { Vector3f( 0, 0, 0 ), Vector3f( 0, 0, 0 ) };
}