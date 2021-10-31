#include "cvar_serverside.hpp"
#include "network/protocol.hpp"

#include "logging.hpp"

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

		protocol::SendPacket( pPeer, p, true );
	}
	void SendServerChunkData( ENetPeer *pPeer, CWorld *world, CVector pos )
	{
		ChunkData crep;
		crep = world->ChunkAtWorldPos( pos )->m_data;

		SendServerChunkDataFromRep( pPeer, crep );
	}
	void SendServerChunkDataFromRep( ENetPeer *pPeer, ChunkData crep )
	{
		ServerPacket p;
		p.type					   = ServerPacket::CHUNKDATA;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << crep.x;
		bufAcc << crep.y;
		bufAcc << crep.z;
		bufAcc << CHUNKLENGTH;
		bufAcc << crep.m_iBlocks;
		bufAcc << crep.m_iValue;

		// con_debug("SEND <%d,%d,%d>", crep.x, crep.y, crep.z);

		SendPacket( pPeer, p, true );
	}
	void SendServerUpdateBlock( ENetPeer *pPeer, CVector pos, BLOCKID blockType, uint16_t val )
	{
		ServerPacket p;
		p.type					   = ServerPacket::UPDATE_BLOCK;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << pos.x;
		bufAcc << pos.y;
		bufAcc << pos.z;
		bufAcc << (uint)blockType;
		bufAcc << val;

		protocol::SendPacket( pPeer, p, true );
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

		SendPacket( pPeer, p, true );
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

		SendPacket( pPeer, p, true );
	}
	void SendServerPlayerDisconnect( ENetPeer *pPeer, bool isKick, std::string reason )
	{
		ServerPacket p;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		p.type						  = ServerPacket::PLAYER_DISCONNECT;

		bufAccess << isKick;
		bufAccess << reason;

		protocol::SendPacket( pPeer, p, true );
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

		protocol::SendPacket( pPeer, p, true );
	}
	void SendServerPlayerSkin( ENetPeer *pPeer, std::string username, std::vector<unsigned char> imageData,
							   unsigned int resolution )
	{
		ServerPacket p;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		p.type						  = ServerPacket::PLAYERSKIN;

		bufAccess << username;
		bufAccess << imageData;
		bufAccess << resolution;

		protocol::SendPacket( pPeer, p, true );
	}
	void SendServerSpecialEffect( ENetPeer *pPeer, CVector pos, int id, int attrib )
	{
		ServerPacket p;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		p.type						  = ServerPacket::SPECIALEFFECT;

		bufAccess << pos.x;
		bufAccess << pos.y;
		bufAccess << pos.z;
		bufAccess << id;
		bufAccess << attrib;

		protocol::SendPacket( pPeer, p );
	}
} // namespace protocol