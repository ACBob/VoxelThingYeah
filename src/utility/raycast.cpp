#include "raycast.hpp"
#include "utility/vector.hpp"
#include "world/world.hpp"

CPointedThing::CPointedThing() : block( nullptr ), position( 0 ) {}

CVoxRaycast::CVoxRaycast() : pos( 0 ), dir( 0 ), length( 0 ) {}

CPointedThing CVoxRaycast::Cast( CWorld *chunkMan )
{
	CVector ray = pos;
	CVector oRay; // for figuring out normal

	const float step = 0.01;
	float i			 = 0;

	CBlock *b = nullptr;
	while ( i <= length )
	{
		oRay = ray;
		ray	 = pos + dir * i;
		i += step;
		b = chunkMan->BlockAtWorldPos( ray );

		if ( chunkMan->TestPointCollision( ray ) )
			break;
	}

	CPointedThing p;
	p.position = CVector( ceil( ray.x ), ceil( ray.y ), ceil( ray.z ) );
	p.block	   = b;
	p.normal   = oRay.Floor() - ray.Floor();
	return p;
}