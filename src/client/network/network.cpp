#include "network.hpp"

#include "shared/logging.hpp"

#include "utility/serialize.hpp"

CClient::CClient()
{
    m_pHost = enet_host_create(nullptr, 1, 1, 0, 0);

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

    con_info("Connected to server");
    con_info("Waiting for server to accept connection");
    if (enet_host_service(m_pHost, &m_event, 5000) > 0)
    {
        if (m_event.type == ENET_EVENT_TYPE_CONNECT)
        {
            con_info("Hello! We've connected to the server!");
            con_info("Sending necessary information to server");
            m_pServer->m_pPeer = m_event.peer;
            network::cl_sendJoinGame(m_pServer->m_pPeer, "Deez", "Deez");
            return true;
        }
        else
        {
            con_error("Connection refused");
            return false;
        }
    }
    else
    {
        con_error("Connection timed out :(");
        return false;
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
    ENetPacket *createClientPacket(char packetType, char* data, unsigned short dataSize)
    {
        CSerializer serializer;
        serializer << (char)MEEGREEF_PROTOCOL_VERSION << packetType << dataSize;
        serializer.Write(data, dataSize);

        ENetPacket *packet = enet_packet_create(serializer.m_chBuffer, serializer.m_nBufferSize, ENET_PACKET_FLAG_RELIABLE);
        return packet;
    }

    void cl_sendJoinGame(ENetPeer* peer, const std::string &name, const std::string &password)
    {
        CSerializer serializer;
        serializer << name << password;

        ENetPacket *packet = createClientPacket(ClientPacket::JOIN_GAME, serializer.m_chBuffer, serializer.m_nBufferSize);
        enet_peer_send(peer, 0, packet);
    }

    void cl_sendLeaveGame(ENetPeer* peer)
    {
        ENetPacket *packet = createClientPacket(ClientPacket::LEAVE_GAME, nullptr, 0);
        enet_peer_send(peer, 0, packet);
    }
}