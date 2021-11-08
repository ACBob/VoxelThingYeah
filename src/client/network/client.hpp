#include "enet/enet.h"
#include "entities/entityplayer.hpp"
#include "network/protocol.hpp"
#include "particles/particlesystem.hpp"
#include "world/world.hpp"

#include "specialeffectdef.hpp"

#pragma once

// This is us.
class CNetworkClient
{
  public:
	CNetworkClient();
	~CNetworkClient();

	// The client will do its' best to determine if it can be used or not
	// Returning true if it can be
	bool WorkingClient();

	bool Connect( const char *cAddress = "127.0.0.1", int iPort = 58008 );
	void Disconnect();

	void Update();

	void DecodeChunkData( ArchiveIntermediary data );

	void SpecialEffectHandle( Vector3f pos, SpecialEffect specialEffect, int attrib );

	// Pointer to the local world
	CWorld *m_pLocalWorld = nullptr;
	// Pointer to the local player
	CEntityPlayer *m_pLocalPlayer = nullptr;
	// Pointer to the particle manager
	CParticleManager *m_pParticleMan = nullptr;

	// Please don't modify
	// Returns if we're connected or not
	bool m_bConnected = false;
	ENetPeer *m_pPeer;

	std::vector<std::string> m_chatBuffer;

  private:
	ENetHost *m_pEnetHost = nullptr;
	ENetAddress m_addr;
	ENetEvent e;
};