#include "cvar_clientside.hpp"
#include "network/protocol.hpp"

namespace protocol
{
	void SendClientPlayerID( ENetPeer *peer )
	{
		ClientPacket p;
		p.type						  = ClientPacket::PLAYER_ID;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		bufAccess << PROTOCOL_VERSION;
		bufAccess << std::string( username->GetString() );

		protocol::SendPacket( peer, p );
	}
	void SendClientSetBlock( ENetPeer *peer, Vector pos, blocktype_t blockType )
	{
		ClientPacket p;
		p.type					   = ClientPacket::SET_BLOCK;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << pos.x;
		bufAcc << pos.y;
		bufAcc << pos.z;
		bufAcc << (uint)blockType;

		protocol::SendPacket( peer, p );
	}
	void SendClientPlayerPos( ENetPeer *peer, Vector pos, Vector rot )
	{
		ClientPacket p;
		p.type					   = ClientPacket::PLAYERPOSORT;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << pos.x;
		bufAcc << pos.y;
		bufAcc << pos.z;
		bufAcc << rot.x;
		bufAcc << rot.y;

		SendPacket( peer, p );
	}
	void SendClientChatMessage( ENetPeer *peer, std::string message )
	{
		ClientPacket p;
		p.type						  = ClientPacket::CHATMESSAGE;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		bufAccess << message;

		SendPacket( peer, p );
	}
	void SendClientLeave( ENetPeer *peer )
	{
		ClientPacket p;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		p.type						  = ClientPacket::LEAVE;

		protocol::SendPacket( peer, p );
		enet_peer_disconnect( peer, 0 );
	}
} // namespace protocol