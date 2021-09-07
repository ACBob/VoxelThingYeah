#include "enet/enet.h"
#include "entities/entityplayer.hpp"
#include "network/protocol.hpp"
#include "world/world.hpp"

class CNetworkPlayer
{
  public:
	ENetPeer *peer		 = nullptr;
	CEntityPlayer *entity = nullptr;
	std::string username;

	int loadedChunkIDX			   = 0;
	unsigned int nextChunkLoadTick = 0;
	CVector chunkPos; // the chunk we're in
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

	CWorld world;

	std::vector<CNetworkPlayer *> players;

	CNetworkPlayer *ClientFromPeer( ENetPeer *p );
	CNetworkPlayer *ClientFromUsername( const char *name );

	void KickPlayer( CNetworkPlayer *c, const char *reason );
	void KickPlayer( const char *str, const char *reason );
	void KickPlayer( ENetPeer *p, const char *reason );

	unsigned int currentTick;

  private:
	ENetAddress addr;
	ENetEvent e;
	ENetHost *enetHost = nullptr;
};