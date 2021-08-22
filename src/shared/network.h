// Wrapper around ENet.

#include "enet/enet.h"

#include "world/world.h"

#include "entities/entityplayer.h"

#include <sstream>
#include <map>

// TODO: a nicer place for these
typedef std::stringstream ArchiveBuf;
typedef std::string ArchiveIntermediary;

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
		public:
			ENetPeer *peer;
			EntityPlayer *entity;
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

			std::map<ENetPeer*, Client*> players;

		private:
			ENetAddress addr;
			ENetEvent e;
			ENetHost *enetHost;

			void SendWorld(ENetPeer *peer, Vector pos);
	};
#endif

	struct NetworkPacket
	{
		enum type_t
		{
			CHUNKDATA = 1,
		};

		uint type;
		ArchiveIntermediary data;

		template <typename S>
		void serialize(S& s)
		{
			s & type & data;
		};
	};

	void SendPacket(ENetPeer *peer, NetworkPacket::type_t type, ArchiveIntermediary data);
	NetworkPacket GetPacketBack(ENetPacket *packet);
}