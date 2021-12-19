// -- Raycast
// Casts a ray from a point in space to a point in space.
// Stopped by either a given chunk, or a chunk in the world.

#include "utility/vector.hpp"

// class CChunk;
class CWorld;

class CRaycast
{
  public:
	CRaycast();
	~CRaycast();

	Vector3f m_start;
	float m_fLength;
	Vector3f m_direction;

	// std::pair<Vector3f, Vector3f> cast( CChunk *chunk );
	// std::pair<Vector3f, Vector3f> cast( CChunk *chunk, Vector3f start, Vector3f direction, float length );
	std::pair<Vector3f, Vector3f> cast( CWorld *world );
	static std::pair<Vector3f, Vector3f> cast( CWorld *world, Vector3f start, Vector3f direction, float length );
};