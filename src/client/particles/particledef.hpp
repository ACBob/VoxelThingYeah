// Defines particle properties

#include "utility/vector.hpp"
#include "rendering/texturemanager.hpp"

#pragma once

struct ParticleDef {
	bool bCollision = true;

	// Initial velocity
	CVector vMinExplode = {0,0,0};
	CVector vMaxExplode = {0,0,0};

	CVector vLinear = {0,0,0}; // Velocity applied every frame
	CVector vDrag = {0.05f, 0.05f, 0.05f}; // Velocity has this percent taken in air
	CVector vFriction = {0.07f, 0.07f, 0.07f}; // If collides, this is the percent taken when on floor (In adddition to drag)

	// How long the particle exists for
	float fMinLifetime = 1.0f;
	float fMaxLifetime = 1.0f;

	// The visual size
	CVector vMinSize = {1,1};
	CVector vMaxSize = {1,1};
	CVector vShrinkage = {0,0}; // How much to change size by every tick, negative grows

	// Particle Emitter
	int iEmissionTick = 1; // Create a new particle every thisth tick
	int iMaxParticles = 1024; // The maximum particles to create
	bool bOneShot = true; // Stop creating if we've reached our max

	// Texture to render with
	CTexture *pTexture = nullptr;
};

const ParticleDef PARTICLE_BREAKBLOCK = {
	true,
	{-5,-5,-5},
	{5,5,5},
	{0.0, -.098, 0.0},
	{0.05f, 0.05f, 0.05f},
	{0.07f, 0.07f, 0.07f},
	0.7f,
	1.3f,
	{0.1f,0.1f,0.1f},
	{0.2f,0.2f,0.2f},
	{0,0},
	1,
	1024,
	true,
	nullptr
};