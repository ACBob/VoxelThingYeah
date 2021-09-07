#include "enet/enet.h"
#include "world/world.hpp"
#include "entities/entityplayer.hpp"
#include "protocol.hpp"

// This is us.
class NetworkClient
{
	public:
		NetworkClient();
		~NetworkClient();

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