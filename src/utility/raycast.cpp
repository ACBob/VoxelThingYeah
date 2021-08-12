#include "utility/vector.h"
#include "world/chunkmanager.h"
#include "raycast.h"

PointedThing::PointedThing() :
	block(nullptr),
	position(0)
{

}

VoxRaycast::VoxRaycast() :
	pos(0),
	dir(0),
	length(0)
{

}

PointedThing VoxRaycast::Cast(ChunkManager *chunkMan)
{
	Vector ray = pos;

	const float step = 0.01;
	float i = 0;
	
	Block *b = nullptr;
	while((b == nullptr || b->blockType == AIR) && i <= length)
	{
		ray = pos + dir * i;
		i += step;
		b = chunkMan->BlockAtWorldPos(ray);
	}

	PointedThing p;
	p.position = Vector(
		ceil(ray.x),
		ceil(ray.y),
		ceil(ray.z)
	);
	p.block = b;
	p.normal = Vector(0,1,0);
	return p;
}