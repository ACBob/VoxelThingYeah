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

    m_pHost = enet_host_create( &m_address, maxClients, 1, 0, 0);
    if (m_pHost == nullptr)
    {
        con_error("Failed to create ENet server");
        return;\
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
    ENetEvent event;
    while ( enet_host_service( m_pHost, &event, 0 ) > 0 )
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                // Clean up
                con_info("Client disconnected");

                // Remove client from list
                CClient *pClient = m_peerMap[event.peer];
                m_clients.erase(std::remove(m_clients.begin(), m_clients.end(), pClient), m_clients.end());
                m_clientMap.erase(pClient->m_sName);
                m_peerMap.erase(event.peer);

                con_info("Goodbye, %s!", pClient->m_sName.c_str());

                // Remove entity
                if (pClient->m_pEntity != nullptr)
                {
                    pClient->m_pEntity->kill();
                    pClient->m_pEntity = nullptr;
                }
            }
            break;
            case ENET_EVENT_TYPE_CONNECT:
            {
                con_info("Client connected");

                // Create client
                CClient *pClient = new CClient();
                pClient->m_pEntity = nullptr;
                pClient->m_pPeer = event.peer;
                pClient->m_sName = "";

                // Add client to list
                m_clients.push_back(pClient);
                m_clientMap[pClient->m_sName] = pClient;
                m_peerMap[event.peer] = pClient;
            }
            break;
            case ENET_EVENT_TYPE_RECEIVE:
            {
                // Get client
                CClient *pClient = m_peerMap[event.peer];

                // Plop the data into a string
                std::string data(event.packet->data, event.packet->data + event.packet->dataLength);

                // Go forth, and handle it my child
                HandlePacket(pClient, data);
            }
            break;
        }
    }

    // Send the position of all players to every other client
    for (auto client : m_clients)
    {
        if (client->m_pEntity != nullptr)
        {
            for (auto otherClient : m_clients)
            {
                if (otherClient != client)
                {
                    // TODO: pitch and yaw
                    network::sv_sendMovePlayer(otherClient->m_pPeer, client->m_sName, client->m_pEntity->getposition(), 0, 0);
                }
            }
        }
    }
}

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

void CServer::KickPlayer( std::string name, std::string reason )
{
    // Get client
    CClient *pClient = m_clientMap[name];

    // Kick the client
    KickPlayer(pClient, reason);
}

void CServer::KickPlayer( CClient *pClient, std::string reason )
{
    // Send the kick reason
    network::sv_sendDisconnect(pClient->m_pPeer, reason);

    // Disconnect the client
    enet_peer_disconnect_now(pClient->m_pPeer, 0);
}

void CServer::KickPlayer( ENetPeer* peer, std::string reason )
{
    // Get client
    CClient *pClient = m_peerMap[peer];

    // Kick the client
    KickPlayer(pClient, reason);
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

                con_info("%s has joined the game", username.c_str());

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

                // Give them an entity
                pClient->m_pEntity = pServer->m_pWorld->createEntity<CPlayerEntity>();
                
                // Set the entity's position
                pClient->m_pEntity->setposition(Vector3f(0, 0, 0));

                sv_sendJoinGameResponse(pClient->m_pPeer, sv_name->GetString(), sv_desc->GetString(), false, pServer->m_pWorld->getChunkSize());

                // Now tell everyone else about the new player
                for (auto& client : pServer->m_clients)
                {
                    if (client != pClient)
                    {
                        sv_sendSpawnPlayer(client->m_pPeer, pClient->m_sName, pClient->m_pEntity->getposition(), 0, 0);

                        // And tell the new player about everyone else
                        sv_sendSpawnPlayer(pClient->m_pPeer, client->m_sName, client->m_pEntity->getposition(), 0, 0);
                    }
                }

            }
            break;
            default:
                con_error("Unknown packet type: %d", packet.m_chPacketType);
            break;
        }
    }

    ENetPacket *createServerPacket(char packetType, char* data, unsigned short dataSize)
    {
        CSerializer serializer;
        serializer << (char)MEEGREEF_PROTOCOL_VERSION << packetType << dataSize;
        serializer.Write(data, dataSize);

        ENetPacket *packet = enet_packet_create(serializer.GetBuffer(), serializer.m_nBufferSize, ENET_PACKET_FLAG_RELIABLE);
        return packet;
    }

    void sv_sendJoinGameResponse(ENetPeer* peer, const std::string& serverName, const std::string& serverMotd, bool isPriviledged, Vector3i chunkSize)
    {
        CSerializer serializer;
        serializer << serverName << serverMotd << isPriviledged << chunkSize.x << chunkSize.y << chunkSize.z;

        ENetPacket *packet = createServerPacket(ServerPacket::JOIN_GAME_RESPONSE, serializer.m_chBuffer, serializer.m_nBufferSize);
        enet_peer_send(peer, 0, packet);
    }

    void sv_sendSpawnPlayer(ENetPeer* peer, const std::string &name, Vector3f position, float pitch, float yaw)
    {
        CSerializer serializer;
        serializer << name << position.x << position.y << position.z << pitch << yaw;

        ENetPacket *packet = createServerPacket(ServerPacket::SPAWN_PLAYER, serializer.m_chBuffer, serializer.m_nBufferSize);
        enet_peer_send(peer, 0, packet);
    }

    void sv_sendMovePlayer(ENetPeer* peer, const std::string& username, Vector3f pos, float pitch, float yaw)
    {
        CSerializer serializer;
        serializer << username << pos.x << pos.y << pos.z << pitch << yaw;

        ENetPacket *packet = createServerPacket(ServerPacket::MOVE_PLAYER, serializer.m_chBuffer, serializer.m_nBufferSize);
        enet_peer_send(peer, 0, packet);
    }

    void sv_sendDisconnect(ENetPeer* peer, const std::string &reason)
    {
        CSerializer serializer;
        serializer << reason;

        ENetPacket *packet = createServerPacket(ServerPacket::DISCONNECT, serializer.m_chBuffer, serializer.m_nBufferSize);
        enet_peer_send(peer, 0, packet);
    }
}