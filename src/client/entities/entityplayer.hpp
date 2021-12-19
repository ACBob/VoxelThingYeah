// -- Entity Player
// The player entity.

#include "shared/entities/entityplayer.hpp"
#include "entitybase.hpp"

class CInputManager;

class CClientEntityPlayer : public CClientEntityBase, public CEntityPlayer
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