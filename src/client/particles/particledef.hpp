// Defines particle properties

#include "rendering/texturemanager.hpp"
#include "utility/vector.hpp"

#pragma once

struct ParticleDef
{
	bool bCollision = true;

	// Initial velocity
	Vector3f vMinExplode = { 0, 0, 0 };
	Vector3f vMaxExplode = { 0, 0, 0 };

	Vector3f vLinear	  = { 0, 0, 0 };			 // Velocity applied every frame
	Vector3f vDrag	  = { 0.05f, 0.05f, 0.05f }; // Velocity has this percent taken in air
	Vector3f vFriction = { 0.07f, 0.07f,
						  0.07f }; // If collides, this is the percent taken when on floor (In adddition to drag)

	// How long the particle exists for
	float fMinLifetime = 1.0f;
	float fMaxLifetime = 1.0f;

	// The visual size
	Vector3f vMinSize   = { 1, 1 };
	Vector3f vMaxSize   = { 1, 1 };
	Vector3f vShrinkage = { 0, 0 }; // How much to change size by every tick, negative grows

	// Particle Emitter
	float fEmissionRate = 1;  // Rate at which to create
	int iEmissionSize = 1; // How many particles to create per rate
	int iMaxParticles = 1024; // The maximum particles to create
	bool bOneShot	  = true; // Stop creating if we've reached our max

	// Texture to render with
	CTexture *pTexture = nullptr;

	Vector4f vUVOffsetMin = { 1, 1, 0, 0 };
	Vector4f vUVOffsetMax = { 1, 1, 0, 0 };
};

const ParticleDef PARTICLE_BREAKBLOCK = { true,
										  { -5, -5, -5 },
										  { 5, 5, 5 },
										  { 0.0, -.4, 0.0 },
										  { 0.05f, 0.05f, 0.05f },
										  { 0.07f, 0.07f, 0.07f },
										  0.7f,
										  1.3f,
										  { 0.05f, 0.05f, 0.05f },
										  { 0.25f, 0.25f, 0.25f },
										  { 0, 0 },
										  1,
										  1,
										  1024,
										  true,
										  nullptr,
										  { 1, 1, 0, 0 },
										  { 1, 1, 0, 0 } };