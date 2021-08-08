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

PointedThing VoxRaycast::Cast(ChunkManager *chunkMan)
{

	Vector ray = pos.Floor();
	Vector step = {
		dir.x > 0.0f ? 1.0f : dir.x < 0.0f ? -1.0f : 0.0f,
		dir.y > 0.0f ? 1.0f : dir.y < 0.0f ? -1.0f : 0.0f,
		dir.z > 0.0f ? 1.0f : dir.z < 0.0f ? -1.0f : 0.0f
	};
	Vector t = dir.Floor();
	Vector delta = {
		step.x / dir.x,
		step.y / dir.y,
		step.z / dir.z
	};

	float radius = length / dir.Magnitude();

	Block *b = nullptr;
	while (ray.x > -MAXCOORD_X && ray.x < MAXCOORD_X && ray.y -MAXCOORD_Y && ray.y < MAXCOORD_Y && ray.z > -MAXCOORD_Z && ray.z < MAXCOORD_Z)
	{
		if (t.x < t.y)
		{
			if (t.x < t.z)
			{
				if (t.x > radius) break;
				ray.x += step.x;
				t.x += delta.x;
			}
			else
			{
				if (t.z > radius) break;
				ray.z += step.z;
				t.z += delta.z;
			}
		}
		else
		{
			if (t.y < t.z)
			{
				if (t.y > radius) break;
				ray.y += step.y;
				t.y += delta.y;
			}
			else
			{
				if (t.z > radius) break;
				ray.z += step.z;
				t.z += delta.z;
			}
		}

		b = chunkMan->BlockAtWorldPos(ray);
		if (b != nullptr && b->blockType != blocktype_t::AIR)
			break;
	}

	PointedThing p;
	p.position = Vector(
		ceil(ray.x),
		ceil(ray.y),
		ceil(ray.z)
	);
	p.block = b;
	return p;
}