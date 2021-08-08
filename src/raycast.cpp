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
// Refernce for implementation based on 'Keven Reid's javascript one
// https://gamedev.stackexchange.com/a/49423

int intbound(float s, float ds) {
	// Edge case
	bool isInt = floor(s) == s;
	if (ds < 0 && isInt)
		return 0;

	return (ds > 0 ? ceil(s) - s : s - floor(s)) / abs(ds);
}

PointedThing VoxRaycast::Cast(ChunkManager *chunkMan)
{
	bool search = true;

	Vector ray = pos.Floor();

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
		int(step.x / dir.x),
		int(step.y / dir.y),
		int(step.z / dir.z)
	);


	// We're not pointing anywhere, don't search
	if (dir == Vector(0))
		search = false;

	float radius = length / dir.Magnitude();

	Block *b = nullptr;
	while (search && ray.x > -MAXCOORD_X && ray.x < MAXCOORD_X && ray.y -MAXCOORD_Y && ray.y < MAXCOORD_Y && ray.z > -MAXCOORD_Z && ray.z < MAXCOORD_Z)
	{
		b = chunkMan->BlockAtWorldPos(ray);
		if (b == nullptr)
			break;
		else if (b->blockType != blocktype_t::AIR)
			break;

		if (tMax.x < tMax.y)
		{
			if (tMax.x < tMax.z)
			{
				ray.x = ray.x + step.x;
				if (tMax.x > radius)
				{
					break;
				}
				tMax.x = tMax.x + delta.x;
			}
			else
			{
				ray.z = ray.z + step.z;
				if (tMax.z > radius)
				{
					break;
				}
				tMax.z = tMax.z + delta.z;
			}
		}
		else
		{
			if (tMax.x < tMax.z)
			{
				ray.y = ray.y + step.y;
				if (tMax.y > radius)
				{
					break;
				}
				tMax.y = tMax.y + delta.y;
			}
			else
			{
				ray.z = ray.z + step.z;
				if (tMax.z > radius)
				{
					break;
				}
				tMax.z = tMax.z + delta.z;
			}
		}
	}

	PointedThing p;
	p.position = Vector(
		ceil(ray.x),
		ceil(ray.y),
		ceil(ray.z)
	);
	// if (b == nullptr)
	// 	b = chunkMan->BlockAtWorldPos(p.position);
	p.block = b;
	return p;
}