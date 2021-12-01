#include "entitycamera.hpp"

CEntityCamera::CEntityCamera() {}
CEntityCamera::~CEntityCamera() {}

void CEntityCamera::PhysicsTick( float delta, CWorld *world )
{
	BaseClass::PhysicsTick( delta, world );
}