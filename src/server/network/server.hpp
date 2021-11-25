#pragma once

#include "enet/enet.h"
#include "entities/entityplayer.hpp"
#include "network/protocol.hpp"
#include "world/world.hpp"

class CNetworkPlayer
{
  public:
	~CNetworkPlayer();

	ENetPeer *m_pPeer		 = nullptr;
	CEntityPlayer *m_pEntity = nullptr;
	std::string m_username;

	// std::vector<unsigned char> m_skinData;
	// unsigned int m_skinRes;

	// A queue of chunks to be sent
	std::vector<Vector3f> m_pChunkQueue;
	// The chunks that have been sent
	std::vector<Vector3f> m_pChunkSent;

	int m_iSpiralX = 0;
	int m_iSpiralZ = 0;
	int m_iSpiralDX = 0;
	int m_iSpiralDZ = 0;

	int m_iLoadedChunkIDX		  = 0;
	uint64_t m_iNextChunkLoadTick = 0;
	Vector3f m_vChunkPos; // the chunk we're in
};

class CNetworkServer
{
  public:
	CNetworkServer( int port = 58008, int maxClients = 8 );
	~CNetworkServer();

	void Update();

	// The server will do its' best to determine if it can be used or not
	// Returning true if it can be
	bool WorkingServer();

	CWorld m_world;

	std::vector<CNetworkPlayer *> m_players;

	CNetworkPlayer *ClientFromPeer( ENetPeer *p );
	CNetworkPlayer *ClientFromUsername( const char *name );

	void KickPlayer( CNetworkPlayer *c, const char *reason );
	void KickPlayer( const char *str, const char *reason );
	void KickPlayer( ENetPeer *p, const char *reason );

	void PlaySoundEvent( const char *str, Vector3f pos );

	unsigned int m_iCurrentTick;

  private:
	ENetAddress m_addr;
	ENetEvent m_e;
	ENetHost *m_pEnetHost = nullptr;
};