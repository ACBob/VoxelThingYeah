#include "enet/enet.h"
#include "world/world.hpp"
#include "entities/entityplayer.hpp"
#include "network/protocol.hpp"

class NetworkPlayer
{
	public:
		ENetPeer *peer = nullptr;
		EntityPlayer *entity = nullptr;
		std::string username;

		int loadedChunkIDX = 0;
		unsigned int nextChunkLoadTick = 0;
		Vector chunkPos; // the chunk we're in
};

class NetworkServer
{
	public:
		NetworkServer(int port = 58008, int maxClients = 8);
		~NetworkServer();

		void Update();

		// The server will do its' best to determine if it can be used or not
		// Returning true if it can be
		bool WorkingServer();

		World world;

		std::vector<NetworkPlayer*> players;
		
		NetworkPlayer *ClientFromPeer(ENetPeer* p);
		NetworkPlayer *ClientFromUsername(const char *name);

		void KickPlayer(NetworkPlayer *c, const char *reason);
		void KickPlayer(const char *str, const char *reason);
		void KickPlayer(ENetPeer *p, const char *reason);

		unsigned int currentTick;

	private:
		ENetAddress addr;
		ENetEvent e;
		ENetHost *enetHost = nullptr;
};