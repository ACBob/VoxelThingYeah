#include "cvar_clientside.hpp"
#include "network/protocol.hpp"

namespace protocol
{
	void SendClientPlayerID( ENetPeer *pPeer )
	{
		ClientPacket p;
		p.type						  = ClientPacket::PLAYER_ID;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		bufAccess << PROTOCOL_VERSION;
		bufAccess << std::string( username->GetString() );

		protocol::SendPacket( pPeer, p );
	}
	void SendClientSetBlock( ENetPeer *pPeer, CVector pos, blocktype_t blockType )
	{
		ClientPacket p;
		p.type					   = ClientPacket::SET_BLOCK;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << pos.x;
		bufAcc << pos.y;
		bufAcc << pos.z;
		bufAcc << (uint)blockType;

		protocol::SendPacket( pPeer, p );
	}
	void SendClientPlayerPos( ENetPeer *pPeer, CVector pos, CVector rot )
	{
		ClientPacket p;
		p.type					   = ClientPacket::PLAYERPOSORT;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << pos.x;
		bufAcc << pos.y;
		bufAcc << pos.z;
		bufAcc << rot.x;
		bufAcc << rot.y;

		SendPacket( pPeer, p );
	}
	void SendClientChatMessage( ENetPeer *pPeer, std::string message )
	{
		ClientPacket p;
		p.type						  = ClientPacket::CHATMESSAGE;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		bufAccess << message;

		SendPacket( pPeer, p );
	}
	void SendClientLeave( ENetPeer *pPeer )
	{
		ClientPacket p;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		p.type						  = ClientPacket::LEAVE;

		protocol::SendPacket( pPeer, p );
		enet_peer_disconnect( pPeer, 0 );
	}
} // namespace protocol