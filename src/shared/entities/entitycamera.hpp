#include "entitybase.hpp"

#pragma once

class CEntityCamera : public CEntityBase
{
	DeclBaseClass( CEntityBase );

  public:
	CEntityCamera();
	~CEntityCamera();

	void Spawn( CWorld *world ) { BaseClass::Spawn( world ); };
	void Kill() { BaseClass::Kill(); };

#ifdef CLIENTEXE
	void UpdateClient( CWorld *clientSideWorld ) { BaseClass::UpdateClient( clientSideWorld ); };

	void Render(){};
	void SetShader( CShader *shader ){};

	float m_fEyeHeight = 0.0f;
#endif

	void PhysicsTick( float delta, CWorld *world );
};