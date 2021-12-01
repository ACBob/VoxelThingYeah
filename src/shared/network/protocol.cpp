#include "protocol.hpp"

#include "network.hpp"

#include "logging.hpp"

namespace protocol
{
	void SendPacket( ENetPeer *pPeer, NetworkPacket &p, bool important )
	{
		ArchiveBuf buf;
		Archive<ArchiveBuf> bufAccess( buf );
		p.data = p.buffer.str();
		bufAccess << p;

		ArchiveIntermediary g = buf.str();

		ENetPacket *packet = enet_packet_create( &g.begin()[0], g.size(), important ? ENET_PACKET_FLAG_RELIABLE : 0 );
		enet_peer_send( pPeer, 0, packet );
	}

	void UncompressAndDealWithPacket( ArchiveIntermediary packetData, void *side, ENetPeer *pPeer )
	{
		NetworkPacket p;
		ArchiveBuf buf( packetData );
		Archive<ArchiveBuf> bufAccess( buf );

		bufAccess >> p;

		DealWithPacket( p, side, pPeer );
	}
} // namespace protocol