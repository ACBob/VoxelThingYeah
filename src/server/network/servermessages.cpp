#include "network/protocolmessages.hpp"
#include "cvar_serverside.h"

namespace protocol
{
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
		crep = world->GetWorldRepresentation(pos);

		SendServerChunkDataFromRep(peer, crep);	
	}
	void SendServerChunkDataFromRep(ENetPeer *peer, World::PortableChunkRepresentation crep)
	{
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
	void SendServerPlayerSpawn  (ENetPeer *peer, std::string username, Vector pos, Vector rot, bool join)
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
		bufAcc << join;

		SendPacket(peer, p);
	}
	void SendServerPlayerPos          (ENetPeer *peer, std::string username, Vector pos, Vector rot)
	{
		ServerPacket p;
		p.type = ServerPacket::PLAYERPOSORT;
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
		ServerPacket p;
		p.type = ServerPacket::CHATMESSAGE;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();

		bufAccess << username;
		bufAccess << message;

		SendPacket(peer, p);			
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
	void SendServerTimeOfDay          (ENetPeer *peer, int ticks)
	{
		ServerPacket p;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		p.type = ServerPacket::TIMEOFDAY;
		
		bufAccess << ticks;

		protocol::SendPacket(peer, p);
	}
}