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

// This algorithm can suck my dick and beyond
// it has been the worst and hardest thing to implement
// I don't care that hblocklsdmkpsdamkp[sdamkp[das
// asd/'#/ASDF fadks,#;'.fasd kl];',fasd l
// ];.adfs .';]#pragma endregionasd#;'.sad .f
// #;dsafasd.;#'/fas ,;l]#'.as ml;[]dfp[]jmknadfsnjkp[]adfs njikp[]fads jklp[]faklp[]
// l']]
// If you would like to fix this for me, please be my guest.

PointedThing VoxRaycast::Cast(ChunkManager *chunkMan)
{

	Vector ray = pos;
	Vector step = {
		// dir.x > 0.0f ? 1.0f : dir.x < 0.0f ? -1.0f : 0.0f,6
		// dir.y > 0.0f ? 1.0f : dir.y < 0.0f ? -1.0f : 0.0f,
		// dir.z > 0.0f ? 1.0f : dir.z < 0.0f ? -1.0f : 0.0f
		float(floor(dir.x * 1000) / 1000),
		float(floor(dir.y * 1000) / 1000),
		float(floor(dir.z * 1000) / 1000)
	};

	if (step.x == 0 || step.y == 0 || step.z == 0)
		NULL; //breakpoint

	Vector t = dir.Floor();
	Vector delta = {
		step.x / dir.x,
		step.y / dir.y,
		step.z / dir.z
	};
	Vector normal;

	float radius = length / dir.Magnitude();

	int i = 0;
	Block *b = nullptr;
	while (ray.x > -MAXCOORD_X && ray.x < MAXCOORD_X && ray.y -MAXCOORD_Y && ray.y < MAXCOORD_Y && ray.z > -MAXCOORD_Z && ray.z < MAXCOORD_Z)
	{
		// Only ever try to cast 1,024 times before quitting (help avoid crash)
		i++;
		if (i > 1024) break;

		if (t.x < t.y)
		{
			if (t.x < t.z)
			{
				if (t.x > radius) break;
				ray.x += step.x;
				t.x += delta.x;
				normal = DirectionVector[step.x > 0 ? Direction::WEST : Direction::EAST];
			}
			else
			{
				if (t.z > radius) break;
				ray.z += step.z;
				t.z += delta.z;
				normal = DirectionVector[step.z > 0 ? Direction::SOUTH : Direction::NORTH];
			}
		}
		else
		{
			if (t.y < t.z)
			{
				if (t.y > radius) break;
				ray.y += step.y;
				t.y += delta.y;
				normal = DirectionVector[step.y > 0 ? Direction::DOWN : Direction::UP];
			}
			else
			{
				if (t.z > radius) break;
				ray.z += step.z;
				t.z += delta.z;
				normal = DirectionVector[step.z > 0 ? Direction::SOUTH : Direction::NORTH];
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
	p.normal = normal;
	return p;
}