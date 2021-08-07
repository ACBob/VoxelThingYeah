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

// Based on "A Fast Voxel Traversal Algorithm for Ray Tracing
// By John Amanatides & Andrew Woo
// http://www.cse.yorku.ca/~amana/research/grid.pdf

int intbound(float s, float ds) {
	if (ds < 0)
		return intbound(-s, -ds);
	
	s = int(s) % 1;
	return (1-s)/ds;
}

PointedThing VoxRaycast::Cast(ChunkManager *chunkMan)
{
	bool search = true;

	Vector ray(pos);

	Vector step(
		dir.x > 0 ? 1 : dir.x < 0 ? -1 : 0,
		dir.y > 0 ? 1 : dir.y < 0 ? -1 : 0,
		dir.z > 0 ? 1 : dir.z < 0 ? -1 : 0
	);

	Vector tMax(
		intbound(pos.x, dir.x),
		intbound(pos.y, dir.y),
		intbound(pos.z, dir.z)
	);

	Vector delta(
		step.x / dir.x,
		step.y / dir.y,
		step.z / dir.z
	);


	// We're not pointing anywhere, don't search
	if (dir == Vector(0))
		search = false;

	float radius = length / dir.Magnitude();

	Block *b;
	while (search)
	{
		if (tMax.x < tMax.y)
		{
			if (tMax.x < tMax.z)
			{
				if (tMax.x > radius) search = false;

				ray.x = ray.x + step.x;
				tMax.x = tMax.x + delta.x;
			}
			else
			{
				if (tMax.x < tMax.z)
				{
					if (tMax.y > radius) search = false;

					ray.y = ray.y + step.y;
					tMax.y = tMax.y + delta.y;
				}
				else
				{
					if (tMax.z > radius) search = false;

					ray.z = ray.z + step.z;
					tMax.z = tMax.z + delta.z;
				}
			}
		}

		b = chunkMan->BlockAtWorldPos(ray);
		if (b == nullptr)
			search = false;
		else if (b->blockType != blocktype_t::AIR)
		{
			search = false;
		}
	}

	PointedThing p;
	p.block = b;
	p.position = pos;
	return p;
}