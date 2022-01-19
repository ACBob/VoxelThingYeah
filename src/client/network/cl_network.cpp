#include "cl_network.hpp"

#include "shared/logging.hpp"

#include "utility/serialize.hpp"

#include <cstring>

#include "cvar_clientside.hpp"

CClient::CClient()
{
    enet_initialize();
    atexit(enet_deinitialize);

    m_pHost = enet_host_create(NULL, 1, 2, 0, 0);

    if (m_pHost == nullptr)
    {
        con_error("Failed to create ENet client");
        return;
    }
}
CClient::~CClient()
{
    enet_host_destroy(m_pHost);
}

bool CClient::Connect(std::string host, int port)
{
    if (m_pServer != nullptr)
    {
        con_error("Client already connected to server");
        return false;
    }

    if (enet_address_set_host_ip(&m_address, host.c_str()) != 0)
    {
        con_error("Failed to resolve hostname");
        return false;
    }
    m_address.port = port;

    con_info("Connecting to %s:%d", host.c_str(), port);

    m_pServer = new SServer();
    m_pServer->m_pPeer = enet_host_connect(m_pHost, &m_address, 1, 0);

    if (m_pServer->m_pPeer == nullptr)
    {
        con_error("Failed to connect to server");
        return false;
    }

    // Wait for connection
    ENetEvent event;
    if (enet_host_service(m_pHost, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        con_info("Connected to server");
        con_info("Send our info");
        network::cl_sendJoinGame(m_pServer->m_pPeer, "test", "test");
        return true;
    }

    con_error("Failed to connect to server");
    return false;
}

void CClient::Update() {
    ENetEvent event;
    while (enet_host_service(m_pHost, &event, 0) > 0)
    {
        con_info("Event: %d", event.type);
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                con_info("Connected to server");
                network::cl_sendJoinGame(m_pServer->m_pPeer, "test", "test");
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                // we need to deserialize the packet before we can use it
                CSerializer serializer((char*)event.packet->data, event.packet->dataLength);
                ServerPacket packet;

                packet.m_usPacketSize = event.packet->dataLength - 2;

                serializer.ReadUChar(packet.m_chVersion);
                serializer.ReadUChar(packet.m_chPacketType);
                
                packet.m_chPacketData = new char[packet.m_usPacketSize];
                serializer.ReadBytes(packet.m_chPacketData, packet.m_usPacketSize);

                // Handle packet
                network::handlePacket(packet, event.peer, this);
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                con_info("Disconnected from server");
                break;
            }
        }
    }
}

void CClient::Disconnect()
{
	con_info( "Disconnecting" );
    network::cl_sendLeaveGame(m_pServer->m_pPeer);

	while ( enet_host_service( m_pHost, &m_event, 0 ) > 0 )
	{
		switch ( m_event.type )
		{
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy( m_event.packet );
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				con_info( "Disconnected. Bye bye server! :)" );
				break;
		}
	}

	enet_peer_reset( m_pServer->m_pPeer );
    m_pServer->m_pPeer = nullptr;
    
    delete m_pServer;
}

namespace network
{
    void handlePacket(const ServerPacket& packet, ENetPeer* peer, void* side)
    {
        CClient *pClient = (CClient*)side;

        switch(packet.m_chPacketType) {
            case ServerPacket::JOIN_GAME_RESPONSE:
                {
                    unsigned char chVersion;
                    std::string serverName;
                    std::string serverMotd;
                    bool bPrivilaged;
                    Vector3c chunkSizes;

                    CSerializer serializer((char*)packet.m_chPacketData, packet.m_usPacketSize);
                    serializer.ReadUChar(chVersion);
                    serializer.ReadSTDString(serverName);
                    serializer.ReadSTDString(serverMotd);
                    serializer.ReadBool(bPrivilaged);

                    serializer.ReadChar(chunkSizes.x);
                    serializer.ReadChar(chunkSizes.y);
                    serializer.ReadChar(chunkSizes.z);

                    con_info("Server info: %s", serverName.c_str());
                    con_info("Server MOTD: %s", serverMotd.c_str());

                    // Set the appropriate cvars
                    cl_servername->SetString(serverName.c_str());

                    // pClient->m_pWorld->SetChunkSizes(chunkSizes);
                }
            break;
        }
    }

    ENetPacket *giveMeAPacket(char packetType, char* data, unsigned short dataSize)
    {
        // dataSize is the packet's 'data' area
        // So we want to add the other fields (Version (1), PacketSize (2), PacketType (1))
        // Create packet

        // We need to serialize the data
        CSerializer serializer;
        serializer.WriteUChar(MEEGREEF_PROTOCOL_VERSION);
        serializer.WriteUChar(packetType);
        serializer.WriteBytes(data, dataSize);

        // Then create the packet
        ENetPacket *packet = enet_packet_create(serializer.GetBuffer(), serializer.GetBufferSize(), ENET_PACKET_FLAG_RELIABLE);

        std::string debug;
        // append the hex values
        for (int i = 0; i < serializer.GetBufferSize(); i++)
        {
            debug += std::to_string((int)serializer.GetBuffer()[i]) + " ";
        }

        con_debug("Packet: %s", debug.c_str());

        return packet;
    }

    void cl_sendJoinGame(ENetPeer* peer, const std::string &name, const std::string &password)
    {
        // Serialize data
        CSerializer serializer;
        serializer << name << password;

        // // DEBUG: print the buffer as hex
        // con_debug("Sending join game packet");
        // con_debug("%s, %s", name.c_str(), password.c_str());
        // for (int i = 0; i < serializer.m_nBufferSize; i++)
        // {
        //     con_debug("0x%02X", serializer.m_chBuffer[i]);
        // }
        // con_debug("..");

        ENetPacket *packet = giveMeAPacket(ClientPacket::JOIN_GAME, serializer.m_chBuffer, serializer.m_nBufferSize);

        // for (int i = 0; i < packet->dataLength; i++)
        // {
        //     con_debug("0x%02X", packet->data[i]);
        // }

        enet_peer_send(peer, 0, packet);
    }

    void cl_sendLeaveGame(ENetPeer* peer)
    {
        ENetPacket *packet = giveMeAPacket(ClientPacket::LEAVE_GAME, nullptr, 0);
        enet_peer_send(peer, 0, packet);
    }
}