// Meegreef protocol

#include "enet/enet.h"

#include "utility/vector.hpp"

// Version
#define MEEGREEF_PROTOCOL_VERSION 0x01

// Max bytes in a packet
#define MEEGREEF_MAX_PACKET_SIZE 0xFFFF

#define DEFAULT_PORT 27015
#define DEF_MAX_CLIENTS 64

struct NetworkPacket
{
    unsigned char m_chVersion;
    unsigned short m_usPacketSize;
    unsigned char m_chPacketType;
    char m_chPacketData[MEEGREEF_MAX_PACKET_SIZE];
};

struct ClientPacket : public NetworkPacket // Coming FROM the client TO the server
{
    enum PacketType : unsigned char {
        INVALID = 0x00,
        JOIN_GAME = 0x01, // Sent by the client when it wants to join a game
        /*
            {
                unsigned char protocolVersion;
                string username;
                string passwordHash; (optional)
            }
        */
        SET_VOXEL = 0x02, // Sent by the client when it wants to set a voxel
        /*
            {
                int x;
                int y;
                int z;
                unsigned int voxel;
            }
        */
        PLAYER_POS_ORT = 0x03, // Sent periodically by the client to update its position
        /*
            {
                float x;
                float y;
                float z;
                float pitch;
                float yaw;
            }
        */
        CHATMESSAGE = 0x04, // Sent by the client when it wants to send a chat message
        /*
            {
                string message;
            }
        */
        LEAVE_GAME = 0x05, // Sent by the client when it wants to leave the game, is NOT a request, so the client is likely already disconnected when this is received
        /*
            {
                Nothing
            }
        */
        USE_BLOCK = 0x06, // Sent by the client when it wants to interact with a block
        /*
            {
                int x;
                int y;
                int z;
            }
        */
    };
};

struct ServerPacket : public NetworkPacket // Coming FROM the client TO the server
{
    enum PacketType : unsigned char {
        INVALID = 0x00,
        JOIN_GAME_RESPONSE = 0x01, // Sent as a response to a JOIN_GAME request
        /*
            {
                unsigned char protoclVersion;
                string serverName;
                string serverMotd;
                bool isPriviledged;
                unsigned char chunkSize_x;
                unsigned char chunkSize_y;
                unsigned char chunkSize_z;
            }
        */
        CHUNK_DATA = 0x02, // Sent by the server to update the client with chunk data
        /*
            {
                int x;
                int y;
                int z;
                int chunkData[chunkSize_x * chunkSize_y * chunkSize_z];
            }
        */
        UPDATE_VOXEL = 0x03, // Sent by the server to update a voxel
        /*
            {
                int x;
                int y;
                int z;
                unsigned int voxel;
            }
        */
        SPAWN_PLAYER = 0x04, // Sent by the server to spawn a player
        /*
            {
                string username;
                float x;
                float y;
                float z;
                float pitch;
                float yaw;
            }
        */
        MOVE_PLAYER = 0x05, // Sent by the server to update a player's position
        /*
            {
                string username;
                float x;
                float y;
                float z;
                float pitch;
                float yaw;
            }
        */
        CHATMESSAGE = 0x06, // Sent by the server to send a chat message
        /*
            {
                string username;
                string message;
            }
        */
        LEAVE_GAME = 0x07, // Sent by the server to tell other clients that a client has left the game
        /*
            {
                string username;
            }
        */
        DISCONNECT = 0x08, // Sent by the server to tell the client to disconnect, is NOT a request so the server will likely already be disconnected when this is received
        /*
            {
                string reason;
            }
        */
    };
};

namespace network
{
    // These functions are called only by the side that needs them, and only defined by the side that needs them
    // The void* side is a disgusting hack for laziness, but it points towards the CClient or CServer that needs the packet
    void handlePacket(const ClientPacket& packet, ENetPeer* peer, void* side);
    void handlePacket(const ServerPacket& packet, ENetPeer* peer, void* side);

    // Client
    void cl_sendJoinGame(ENetPeer* peer, const std::string& username, const std::string& passwordHash);
    void cl_sendSetVoxel(ENetPeer* peer, Vector3i pos, unsigned int voxel);
    void cl_sendPlayerPosOrt(ENetPeer* peer, Vector3f pos, float pitch, float yaw);
    void cl_sendChatMessage(ENetPeer* peer, const std::string& message);
    void cl_sendLeaveGame(ENetPeer* peer);
    void cl_sendUseBlock(ENetPeer* peer, Vector3i pos);

    // Server
    void sv_sendJoinGameResponse(ENetPeer* peer, const std::string& serverName, const std::string& serverMotd, bool isPriviledged, Vector3i chunkSize);
    void sv_sendChunkData(ENetPeer* peer, Vector3i pos, const unsigned int* chunkData);
    void sv_sendUpdateVoxel(ENetPeer* peer, Vector3i pos, unsigned int voxel);
    void sv_sendSpawnPlayer(ENetPeer* peer, const std::string& username, Vector3f pos, float pitch, float yaw);
    void sv_sendMovePlayer(ENetPeer* peer, const std::string& username, Vector3f pos, float pitch, float yaw);
    void sv_sendChatMessage(ENetPeer* peer, const std::string& username, const std::string& message);
    void sv_sendLeaveGame(ENetPeer* peer, const std::string& username);
    void sv_sendDisconnect(ENetPeer* peer, const std::string& reason);
}