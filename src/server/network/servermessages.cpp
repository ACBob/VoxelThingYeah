#include "cvar_serverside.hpp"
#include "network/protocol.hpp"

namespace protocol
{
	void SendServerPlayerID( ENetPeer *pPeer, bool isOp )
	{
		ServerPacket p;
		p.type					   = ServerPacket::PLAYER_ID;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << PROTOCOL_VERSION;
		bufAcc << std::string( sv_name->GetString() );
		bufAcc << std::string( sv_desc->GetString() );
		bufAcc << isOp;

		protocol::SendPacket( pPeer, p );
	}
	void SendServerChunkData( ENetPeer *pPeer, CWorld *world, CVector pos )
	{
		CWorld::PortableChunkRepresentation crep;
		crep = world->GetWorldRepresentation( pos );

		SendServerChunkDataFromRep( pPeer, crep );
	}
	void SendServerChunkDataFromRep( ENetPeer *pPeer, CWorld::PortableChunkRepresentation crep )
	{
		ServerPacket p;
		p.type					   = ServerPacket::CHUNKDATA;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << crep.x;
		bufAcc << crep.y;
		bufAcc << crep.z;
		bufAcc << CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z;
		bufAcc << crep.blocks;

		SendPacket( pPeer, p );
	}
	void SendServerUpdateBlock( ENetPeer *pPeer, CVector pos, blocktype_t blockType )
	{
		ServerPacket p;
		p.type					   = ServerPacket::UPDATE_BLOCK;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << pos.x;
		bufAcc << pos.y;
		bufAcc << pos.z;
		bufAcc << (uint)blockType;

		protocol::SendPacket( pPeer, p );
	}
	void SendServerPlayerSpawn( ENetPeer *pPeer, std::string username, CVector pos, CVector rot, bool join )
	{
		ServerPacket p;
		p.type					   = ServerPacket::PLAYER_SPAWN;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << username;
		bufAcc << pos.x;
		bufAcc << pos.y;
		bufAcc << pos.z;
		bufAcc << rot.x;
		bufAcc << rot.y;
		bufAcc << join;

		SendPacket( pPeer, p );
	}
	void SendServerPlayerPos( ENetPeer *pPeer, std::string username, CVector pos, CVector rot )
	{
		ServerPacket p;
		p.type					   = ServerPacket::PLAYERPOSORT;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << username;
		bufAcc << pos.x;
		bufAcc << pos.y;
		bufAcc << pos.z;
		bufAcc << rot.x;
		bufAcc << rot.y;

		SendPacket( pPeer, p );
	}
	void SendServerPlayerMessage( ENetPeer *pPeer, std::string username, std::string message )
	{
		ServerPacket p;
		p.type						  = ServerPacket::CHATMESSAGE;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();

		bufAccess << username;
		bufAccess << message;

		SendPacket( pPeer, p );
	}
	void SendServerPlayerDisconnect( ENetPeer *pPeer, bool isKick, std::string reason )
	{
		ServerPacket p;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		p.type						  = ServerPacket::PLAYER_DISCONNECT;

		bufAccess << isKick;
		bufAccess << reason;

		protocol::SendPacket( pPeer, p );
	}
	void SendServerTimeOfDay( ENetPeer *pPeer, int ticks )
	{
		ServerPacket p;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		p.type						  = ServerPacket::TIMEOFDAY;

		bufAccess << ticks;

		protocol::SendPacket( pPeer, p );
	}
	void SendServerPlayerLeave( ENetPeer *pPeer, std::string username )
	{
		ServerPacket p;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		p.type						  = ServerPacket::PLAYERLEAVE;

		bufAccess << username;

		protocol::SendPacket( pPeer, p );
	}
	void SendServerPlayerSkin( ENetPeer *pPeer, std::string username, std::vector<unsigned char> imageData, unsigned int resolution )
	{
		ServerPacket p;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		p.type						  = ServerPacket::PLAYERSKIN;

		bufAccess << username;
		bufAccess << imageData;
		bufAccess << resolution;

		protocol::SendPacket( pPeer, p );
	}
} // namespace protocol