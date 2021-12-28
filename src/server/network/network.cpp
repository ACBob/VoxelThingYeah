#include "network.hpp"

#include "shared/logging.hpp"
#include "shared/network/protocol.hpp"

#include "entities/entities.hpp"

#include "shared/world/world.hpp"

#include <algorithm>

#include "utility/serialize.hpp"

#include "cvar_serverside.hpp"

CClient::~CClient()
{
    enet_peer_disconnect_now(m_pPeer, 0);
}

CServer::CServer(int port, int maxClients)
{
    m_address.host = ENET_HOST_ANY;
    m_address.port = port;

    m_port = port;
    m_maxClients = maxClients;

    con_info("Starting server on port %d", port);

    m_pHost = enet_host_create(nullptr, 1, 2, 0, 0);
    if (m_pHost == nullptr)
    {
        con_error("Failed to create ENet server");
        return;
    }
}

CServer::~CServer()
{
    // Kick all players
    for (auto client : m_clients)
    {
        KickPlayer(client, "kick.serverShutdown");
    }

    // Clean up
    enet_host_destroy(m_pHost);
}

void CServer::Update()
{
    while ( enet_host_service( m_pHost, &m_event, 0 ) > 0 )
    {
        switch (m_event.type)
        {
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                // Clean up
                con_info("Client disconnected");

                // Remove client from list
                CClient *pClient = m_peerMap[m_event.peer];
                m_clients.erase(std::remove(m_clients.begin(), m_clients.end(), pClient), m_clients.end());
                m_clientMap.erase(pClient->m_sName);
                m_peerMap.erase(m_event.peer);

                con_info("Goodbye, %s!", pClient->m_sName.c_str());

                // Remove entity
                if (pClient->m_pEntity != nullptr)
                {
                    pClient->m_pEntity->kill();
                    pClient->m_pEntity = nullptr;
                }
            }
            break;
            case ENET_EVENT_TYPE_RECEIVE:
            {
                // Get client
                CClient *pClient = m_peerMap[m_event.peer];

                // Plop the data into a string
                std::string data(m_event.packet->data, m_event.packet->data + m_event.packet->dataLength);

                // Go forth, and handle it my child
                HandlePacket(pClient, data);
            }
            break;
        }
    }
}

// This next function is very big.
// It decodes the packets and handles them
void CServer::HandlePacket(CClient *pClient, std::string message)
{
    CSerializer serializer((char*)message.c_str(), message.size());

    ClientPacket packet;
    serializer >> packet.m_chVersion >> packet.m_usPacketSize >> packet.m_chPacketType;
    serializer.Read(packet.m_chPacketData, packet.m_usPacketSize);

    // // Check version
    // if (packet.m_chVersion != PROTOCOL_VERSION)
    // {
    //     con_error("Client version mismatch: %d", packet.m_chVersion);
    //     KickPlayer(pClient, "kick.versionMismatch");
    //     return;
    // }

    // // Check packet size
    // if (packet.m_usPacketSize > MAX_PACKET_SIZE)
    // {
    //     con_error("Client packet size too large: %d", packet.m_usPacketSize);
    //     KickPlayer(pClient, "kick.packetSizeTooLarge");
    //     return;
    // }

    network::handlePacket(packet, pClient->m_pPeer, this);
}

// Protocol Implementation
namespace network
{
    void handlePacket(const ClientPacket& packet, ENetPeer* peer, void* side)
    {
        CServer *pServer = (CServer*)side;

        // Get client
        CClient *pClient = pServer->m_peerMap[peer];

        switch (packet.m_chPacketType)
        {
            case ClientPacket::JOIN_GAME:
            {
                std::string username;
                std::string passwordHash;

                CSerializer serializer((char*)packet.m_chPacketData, packet.m_usPacketSize);
                serializer >> username >> passwordHash;

                // Check if the username is already taken
                if (pServer->m_clientMap.find(username) != pServer->m_clientMap.end())
                {
                    // Username is taken
                    con_error("Username %s is already taken", username.c_str());
                    pServer->KickPlayer(pClient, "kick.usernameTaken");
                    return;
                }

                // check if the server is full
                if (pServer->m_clients.size() >= pServer->GetMaxClients())
                {
                    con_error("Server is full");
                    pServer->KickPlayer(pClient, "kick.serverFull");
                    return;
                }

                // TODO: Check if the password is correct

                // Set the client's name
                pClient->m_sName = username;

                // Add the client to the list
                pServer->m_clients.push_back(pClient);
                pServer->m_clientMap[username] = pClient;
                pServer->m_peerMap[pClient->m_pPeer] = pClient;

                // Give them an entity
                pClient->m_pEntity = pServer->m_pWorld->createEntity<CPlayerEntity>();
                
                // Set the entity's position
                pClient->m_pEntity->setposition(Vector3f(0, 0, 0));

                sendJoinGameResponse(pClient->m_pPeer, sv_name->GetString(), sv_desc->GetString(), false, pServer->m_pWorld->getChunkSize());

                // Now tell everyone else about the new player
                for (auto& client : pServer->m_clients)
                {
                    if (client != pClient)
                    {
                        sendSpawnPlayer(client->m_pPeer, pClient->m_sName, pClient->m_pEntity->getposition(), 0, 0);
                    }
                }

            }
        }
    }
}