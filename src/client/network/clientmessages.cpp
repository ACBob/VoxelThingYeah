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

		protocol::SendPacket( pPeer, p, true );
	}
	void SendClientSetBlock( ENetPeer *pPeer, Vector3f pos, BLOCKID blockType, uint16_t blockData )
	{
		ClientPacket p;
		p.type					   = ClientPacket::SET_BLOCK;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << pos.x;
		bufAcc << pos.y;
		bufAcc << pos.z;
		bufAcc << (uint)blockType;
		bufAcc << blockData;

		protocol::SendPacket( pPeer, p, true );
	}
	void SendClientPlayerPos( ENetPeer *pPeer, Vector3f pos, Vector3f rot )
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
		p.type = ClientPacket::LEAVE;

		protocol::SendPacket( pPeer, p, true );
		enet_peer_disconnect( pPeer, 0 );
	}
	void SendClientSkin( ENetPeer *pPeer, std::vector<unsigned char> imageData, unsigned int resolution )
	{
		ClientPacket p;
		Archive<ArchiveBuf> bufAccess = p.GetAccess();
		p.type						  = ClientPacket::SKIN;

		bufAccess << imageData;
		bufAccess << resolution;

		protocol::SendPacket( pPeer, p, true );
	}
	void SendClientUseBlock( ENetPeer *pPeer, Vector3f pos )
	{
		ClientPacket p;
		p.type					   = ClientPacket::USE_BLOCK;
		Archive<ArchiveBuf> bufAcc = p.GetAccess();
		bufAcc << (int)pos.x;
		bufAcc << (int)pos.y;
		bufAcc << (int)pos.z;

		SendPacket( pPeer, p );
	}
} // namespace protocol