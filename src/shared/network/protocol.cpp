#include "protocol.hpp"

#include "utility/serialize.hpp"

namespace network
{
    ENetPacket *createClientPacket(char packetType, char* data, unsigned short dataSize)
    {
        CSerializer serializer;
        serializer << (char)MEEGREEF_PROTOCOL_VERSION << packetType << dataSize;
        serializer.Write(data, dataSize);

        ENetPacket *packet = enet_packet_create(serializer.GetBuffer(), serializer.GetSize(), ENET_PACKET_FLAG_RELIABLE);
        return packet;
    }
}