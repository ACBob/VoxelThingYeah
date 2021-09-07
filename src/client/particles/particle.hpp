#include "vector.hpp"

class CParticle
{

  public:
	CParticle();
	~CParticle();

	CVector position;
	CVector size; // Z Ignored!!!

	bool shouldDie;
};