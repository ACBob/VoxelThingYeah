#include "entitybase.hpp"

#pragma once

class CEntityCamera : public CEntityBase
{
	DeclBaseClass( CEntityBase );

  public:
	CEntityCamera();
	~CEntityCamera();

	void Spawn() { BaseClass::Spawn(); };
	void Kill() { BaseClass::Kill(); };

#ifdef CLIENTEXE
	void UpdateClient( CWorld *clientSideWorld ) { BaseClass::UpdateClient( clientSideWorld ); };
#endif

	void PhysicsTick( float delta, CWorld *world ) { BaseClass::PhysicsTick( delta, world ); };
};