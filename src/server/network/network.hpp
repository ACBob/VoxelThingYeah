#include "enet/enet.h"

#include <map>
#include <vector>
#include <string>

class CPlayerEntity;
class CWorld;

class CClient
{
    public:
        ~CClient();

        ENetPeer *m_pPeer = nullptr;
        CPlayerEntity *m_pEntity = nullptr;
        std::string m_sName;

};

class CServer
{
    public:
        CServer( int port, int maxClients );
        ~CServer();

        void Update();

        CWorld *m_pWorld = nullptr;

        std::vector<CClient *> m_clients;
        std::map<std::string, CClient *> m_clientMap;
        std::map<ENetPeer*, CClient *> m_peerMap;

        void KickPlayer( CClient *pClient, std::string reason );
        void KickPlayer( std::string name, std::string reason );
        void KickPlayer( ENetPeer *pPeer, std::string reason );

        void Broadcast( std::string message );
        
        void Tell( std::string name, std::string message );
        void Tell( CClient *pClient, std::string message );

        void HandlePacket( CClient *pClient, std::string message );

        int GetMaxClients() { return m_maxClients; }

    protected:
        ENetAddress m_address;
        ENetHost *m_pHost = nullptr;
        ENetEvent m_event;
        int m_port;
        int m_maxClients;
};