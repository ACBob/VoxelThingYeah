// -- Entity Player
// The player entity.

#include "shared/entities/entityplayer.hpp"
#include "entitybase.hpp"

class CInputManager;

class CEntityCamera;

class CClientEntityPlayer : public CClientEntityBase, public CEntityPlayer
{
    DeclBaseClass(CClientEntityPlayer, CEntityPlayer);

    public:
    CClientEntityPlayer(entityId_t id, CWorld *pWorld);
    ~CClientEntityPlayer();

    void Update(float dt);

    void SetInputManager(CInputManager* pInputManager);

    private:
    CInputManager* m_pInputManager;
    bool m_bFly;

    CEntityCamera* m_pCamera;
};