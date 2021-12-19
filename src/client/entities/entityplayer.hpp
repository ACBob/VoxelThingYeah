// -- Entity Player
// The player entity.

#include "shared/entities/entityplayer.hpp"

class CInputManager;

class CClientEntityPlayer : public CEntityPlayer
{
    DeclBaseClass(CClientEntityPlayer, CEntityPlayer);

    public:
    CClientEntityPlayer(entityId_t id);
    ~CClientEntityPlayer();

    void Update(float dt);

    void SetInputManager(CInputManager* pInputManager);

    private:
    CInputManager* m_pInputManager;
    bool m_bFly;
};