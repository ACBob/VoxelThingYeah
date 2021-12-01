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

	void PhysicsTick( float delta, CWorld *world );
};