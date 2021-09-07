#include "protocol.hpp"

#include "network.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

#ifdef CLIENTEXE
#include "cvar_clientside.hpp"
#elif SERVEREXE
#include "cvar_serverside.hpp"
#endif

namespace protocol
{
	void SendPacket(ENetPeer *peer, ClientPacket &p)
	{
		ArchiveBuf buf;
		Archive<ArchiveBuf> bufAccess(buf);
		p.data = p.buffer.str();
		bufAccess << p;

		ArchiveIntermediary g = buf.str();

		ENetPacket *packet = enet_packet_create(&g.begin()[0], g.size(), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
	}
	void SendPacket(ENetPeer *peer, ServerPacket &p)
	{
		ArchiveBuf buf;
		Archive<ArchiveBuf> bufAccess(buf);
		p.data = p.buffer.str();
		bufAccess << p;

		ArchiveIntermediary g = buf.str();

		ENetPacket *packet = enet_packet_create(&g.begin()[0], g.size(), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
	}

	void UncompressAndDealWithPacket(ArchiveIntermediary packetData, void *side, ENetPeer *peer)
	{
		NetworkPacket p;
		ArchiveBuf buf(packetData);
		Archive<ArchiveBuf> bufAccess(buf);

		bufAccess >> p;

		DealWithPacket(p, side, peer);
	}
}