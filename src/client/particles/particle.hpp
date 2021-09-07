#include "vector.hpp"

class CParticle
{

  public:
	CParticle();
	~CParticle();

	CVector m_vPosition;
	CVector m_vSize; // Z Ignored!!!

	bool bShouldDie;
};