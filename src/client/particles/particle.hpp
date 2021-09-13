#include "vector.hpp"
#include "rendering/modelmanager.hpp"

class CParticle
{

  public:
	CParticle();
	~CParticle();

	void Render();

	CVector m_vPosition;
	CVector m_vSize; // Z Ignored!!!

	CModel *m_mdl;

	bool m_bShouldDie;
};