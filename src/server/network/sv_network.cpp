#include "sv_network.hpp"

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

    enet_initialize();
    atexit(enet_deinitialize);

    m_port = port;
    m_maxClients = maxClients;

    con_info("Starting server on port %d", port);

    m_pHost = enet_host_create( &m_address, maxClients, 2, 0, 0);
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
    ENetEvent event;
    while ( enet_host_service( m_pHost, &event, 0 ) > 0 )
    {
        con_info("Event: %d", event.type);
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
                // we need to deserialize the packet before we can use it
                CSerializer serializer((char*)event.packet->data, event.packet->dataLength);
                ClientPacket packet;

                packet.m_usPacketSize = event.packet->dataLength - 2;

                serializer.ReadUChar(packet.m_chVersion);
                serializer.ReadUChar(packet.m_chPacketType);
                serializer.ReadBytes(packet.m_chPacketData, packet.m_usPacketSize);

                std::string debug;
                // append hex
                for (int i = 0; i < serializer.m_nBufferSize; i++)
                {
                    debug += std::to_string((int)serializer.m_chBuffer[i]) + " ";
                }

                con_debug("Received packet: %s", debug.c_str());

                // Handle packet
                network::handlePacket(packet, event.peer, this);
            }
            break;

            // Shut up compiler
            case ENET_EVENT_TYPE_NONE: break;
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
        }
    }

    ENetPacket *giveMeAPacket(char packetType, char* data, unsigned short dataSize)
    {
        // dataSize is the packet's 'data' area
        // So we want to add the other fields (Version (1), PacketSize (2), PacketType (1))
        // Create packet
        ENetPacket *packet = enet_packet_create(nullptr, dataSize + 4, ENET_PACKET_FLAG_RELIABLE);

        // Now we need to serialize the data
        CSerializer serializer;
        serializer << (char)MEEGREEF_PROTOCOL_VERSION << dataSize << packetType;
        serializer.WriteBytes(data, dataSize);

        // Now we can copy the serialized data to the packet
        memcpy(packet->data, serializer.GetBuffer(), dataSize + 4);

        return packet;
    }

    void sv_sendJoinGameResponse(ENetPeer* peer, const std::string& serverName, const std::string& serverMotd, bool isPriviledged, Vector3i chunkSize)
    {
        CSerializer serializer;
        serializer << serverName << serverMotd << isPriviledged << chunkSize.x << chunkSize.y << chunkSize.z;

        ENetPacket *packet = giveMeAPacket(ServerPacket::JOIN_GAME_RESPONSE, serializer.m_chBuffer, serializer.m_nBufferSize);
        enet_peer_send(peer, 0, packet);
    }

    void sv_sendSpawnPlayer(ENetPeer* peer, const std::string &name, Vector3f position, float pitch, float yaw)
    {
        CSerializer serializer;
        serializer << name << position.x << position.y << position.z << pitch << yaw;

        ENetPacket *packet = giveMeAPacket(ServerPacket::SPAWN_PLAYER, serializer.m_chBuffer, serializer.m_nBufferSize);
        enet_peer_send(peer, 0, packet);
    }

    void sv_sendMovePlayer(ENetPeer* peer, const std::string& username, Vector3f pos, float pitch, float yaw)
    {
        CSerializer serializer;
        serializer << username << pos.x << pos.y << pos.z << pitch << yaw;

        ENetPacket *packet = giveMeAPacket(ServerPacket::MOVE_PLAYER, serializer.m_chBuffer, serializer.m_nBufferSize);
        enet_peer_send(peer, 0, packet);
    }

    void sv_sendDisconnect(ENetPeer* peer, const std::string &reason)
    {
        CSerializer serializer;
        serializer << reason;

        ENetPacket *packet = giveMeAPacket(ServerPacket::DISCONNECT, serializer.m_chBuffer, serializer.m_nBufferSize);
        enet_peer_send(peer, 0, packet);
    }
}