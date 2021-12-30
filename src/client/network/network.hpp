#include "enet/enet.h"

#include <map>
#include <vector>
#include <string>

#include "shared/network/protocol.hpp"

class CPlayerEntity;

struct SServer
{
    std::string m_sName;
    std::string m_sDesc;

    ENetPeer *m_pPeer = nullptr;
};

class CClient
{
    public:
        CClient();
        ~CClient();

        bool Connect( std::string host, int port );
        void Disconnect();

        void Update();

        SServer *m_pServer = nullptr;
        CPlayerEntity *m_pEntity = nullptr;
        
    protected:
        ENetHost *m_pHost = nullptr;
        ENetAddress m_address;
        ENetEvent m_event;
};