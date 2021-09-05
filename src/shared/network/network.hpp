// Wrapper around ENet.

#include "enet/enet.h"

#include "world/world.hpp"

#include "entities/entityplayer.hpp"

#include <sstream>

#include "protocol.hpp"


namespace network
{

	bool Init();
	void Uninit();

#ifdef CLIENTEXE
	// This is us.
	class Client
	{
		public:
			Client();
			~Client();

			// The client will do its' best to determine if it can be used or not
			// Returning true if it can be
			bool WorkingClient();

			bool Connect(const char* address = "127.0.0.1", int port = 58008);
			void Disconnect();

			void Update();

			void DecodeChunkData(ArchiveIntermediary data);

			// Pointer to the local world
			World* localWorld = nullptr;
			// Pointer to the local player
			EntityPlayer *localPlayer = nullptr;

			// Please don't modify
			// Returns if we're connected or not
			bool connected;
			ENetPeer* peer;

			std::vector<std::string> chatBuffer;

		private:
			ENetHost *enetHost = nullptr;
			ENetAddress addr;
			ENetEvent e;
	};
#elif SERVEREXE
	// A player in the server.
	class Client
	{
		public:
			ENetPeer *peer = nullptr;
			EntityPlayer *entity = nullptr;
			std::string username;

			int loadedChunkIDX = 0;
			unsigned int nextChunkLoadTick = 0;
			Vector chunkPos; // the chunk we're in
	};
#endif

#ifdef CLIENTEXE
	// This the server we've connected to.
	class Server
	{

	};
#elif SERVEREXE
	// This is us.
	class Server
	{
		public:
			Server(int port = 58008, int maxClients = 8);
			~Server();

			void Update();

			// The server will do its' best to determine if it can be used or not
			// Returning true if it can be
			bool WorkingServer();

			World world;

			std::vector<Client*> players;
			
			Client *ClientFromPeer(ENetPeer* p);
			Client *ClientFromUsername(const char *name);

			void KickPlayer(Client *c, const char *reason);
			void KickPlayer(const char *str, const char *reason);
			void KickPlayer(ENetPeer *p, const char *reason);

			unsigned int currentTick;

		private:
			ENetAddress addr;
			ENetEvent e;
			ENetHost *enetHost = nullptr;
	};
#endif
}