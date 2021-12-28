#include "protocol.hpp"

namespace network
{
    void sendPacket(const NetworkPacket& packet, ENetPeer* peer)
    {
        if (peer == nullptr)
        {
            return;
        }

        ENetPacket* pPacket = enet_packet_create(packet.m_chPacketData, packet.m_usPacketSize, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, pPacket);
    }
}