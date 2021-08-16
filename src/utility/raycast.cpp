#include "utility/vector.h"
#include "world/world.h"
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

PointedThing VoxRaycast::Cast(World *chunkMan)
{
	Vector ray = pos;
	Vector oRay; // for figuring out normal

	const float step = 0.01;
	float i = 0;
	
	Block *b = nullptr;
	while(i <= length)
	{
		oRay = ray;
		ray = pos + dir * i;
		i += step;
		b = chunkMan->BlockAtWorldPos(ray);

		if (chunkMan->TestPointCollision(ray))
			break;
	}

	PointedThing p;
	p.position = Vector(
		ceil(ray.x),
		ceil(ray.y),
		ceil(ray.z)
	);
	p.block = b;
	p.normal = oRay.Floor() - ray.Floor();
	return p;
}