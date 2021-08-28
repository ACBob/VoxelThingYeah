#include "protocol.hpp"

#ifdef CLIENTEXE
#include "cvar_clientside.hpp"
#elif SERVEREXE
#include "cvar_serverside.hpp"
#endif


namespace protocol
{
	namespace messages
	{
#ifdef SERVEREXE
		void SendServerPlayerID     (ENetPeer *peer, bool isOp)
		{
			ServerPacket p;
			p.type = ServerPacket::PLAYER_ID;
			Archive<ArchiveBuf> bufAcc = p.GetAccess();
			bufAcc << PROTOCOL_VERSION;
			bufAcc << std::string(sv_name->GetString());
			bufAcc << std::string(sv_desc->GetString());
			bufAcc << isOp;

			protocol::SendPacket(peer, p);
		}
		void SendServerChunkData    (ENetPeer *peer, World *world, Vector pos)
		{
			World::PortableChunkRepresentation crep;
			crep = world->GetWorldRepresentation(Vector(0));

			ServerPacket p;
			p.type = ServerPacket::CHUNKDATA;
			Archive<ArchiveBuf> bufAcc = p.GetAccess();
			bufAcc << crep.x;
			bufAcc << crep.y;
			bufAcc << crep.z;
			bufAcc << CHUNKSIZE_X*CHUNKSIZE_Y*CHUNKSIZE_Z;
			bufAcc << crep.blocks;

			SendPacket(peer, p);
		}
		void SendServerUpdateBlock  (ENetPeer *peer, Vector pos, blocktype_t blockType)
		{
			ServerPacket p;
			p.type = ServerPacket::UPDATE_BLOCK;
			Archive<ArchiveBuf> bufAcc = p.GetAccess();
			bufAcc << pos.x;
			bufAcc << pos.y;
			bufAcc << pos.z;
			bufAcc << (uint)blockType;

			protocol::SendPacket(peer, p);
		}
		void SendServerPlayerSpawn  (ENetPeer *peer, std::string username, Vector pos, Vector rot)
		{
			ServerPacket p;
			p.type = ServerPacket::PLAYER_SPAWN;
			Archive<ArchiveBuf> bufAcc = p.GetAccess();
			bufAcc << username;
			bufAcc << pos.x;
			bufAcc << pos.y;
			bufAcc << pos.z;
			bufAcc << rot.x;
			bufAcc << rot.y;

			SendPacket(peer, p);
		}
		void SendServerPlayerMessage(ENetPeer *peer, std::string username, std::string message)
		{

		}
		void SendServerPlayerDisconnect   (ENetPeer *peer, bool isKick, std::string reason)
		{
			ServerPacket p;
			Archive<ArchiveBuf> bufAccess = p.GetAccess();
			p.type = ServerPacket::PLAYER_DISCONNECT;
			
			bufAccess << isKick;
			bufAccess << reason;

			protocol::SendPacket(peer, p);
		}
#elif CLIENTEXE
		void SendClientPlayerID           (ENetPeer *peer)
		{
			ClientPacket p;
			p.type = ClientPacket::PLAYER_ID;
			Archive<ArchiveBuf> bufAccess = p.GetAccess();
			bufAccess << PROTOCOL_VERSION;
			bufAccess << std::string(username->GetString());

			protocol::SendPacket(peer, p);
		}
		void SendClientSetBlock           (ENetPeer *peer, Vector pos, blocktype_t blockType)
		{
			ClientPacket p;
			p.type = ClientPacket::SET_BLOCK;
			Archive<ArchiveBuf> bufAcc = p.GetAccess();
			bufAcc << pos.x;
			bufAcc << pos.y;
			bufAcc << pos.z;
			bufAcc << (uint)blockType;

			protocol::SendPacket(peer, p);
		}
		void SendClientPlayerPos          (ENetPeer *peer, Vector pos, Vector rot)
		{

		}
		void SendClientChatMessage        (ENetPeer *peer, std::string message)
		{
			
		}
		void SendClientLeave              (ENetPeer *peer)
		{
			ClientPacket p;
			Archive<ArchiveBuf> bufAccess = p.GetAccess();
			p.type = ClientPacket::LEAVE;

			protocol::SendPacket(peer, p);
			enet_peer_disconnect(peer, 0);
		}
#endif
	}
}