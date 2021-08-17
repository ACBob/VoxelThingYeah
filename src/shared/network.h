// Wrapper around ENet.

#include "enet/enet.h"

#include "world/world.h"

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

			void DecodeChunkData(unsigned char *data, unsigned long length);

			// Pointer to the local world
			World* localWorld = nullptr;

		private:
			ENetHost *enetHost;
			ENetAddress addr;
			ENetEvent e;
			ENetPeer* peer;
	};
#elif SERVEREXE
	// A player in the server.
	class Client
	{
		// TODO:
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

			void Update(World *world);

			// The server will do its' best to determine if it can be used or not
			// Returning true if it can be
			bool WorkingServer();

		private:
			ENetAddress addr;
			ENetEvent e;
			ENetHost *enetHost;

			void SendWorld(World *world, ENetPeer *peer, Vector pos);
	};
#endif

}