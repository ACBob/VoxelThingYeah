#include "entityplayer.hpp"

#include "inputmanager.hpp"

CClientEntityPlayer::CClientEntityPlayer(entityId_t id) : BaseClass(id)
{
    m_pInputManager = nullptr;
    m_bFly = true;
}

CClientEntityPlayer::~CClientEntityPlayer()
{
}

void CClientEntityPlayer::SetInputManager(CInputManager* pInputManager)
{
    m_pInputManager = pInputManager;
}

void CClientEntityPlayer::Update(float dt)
{
    BaseClass::Update(dt);

    // We're probably a NPC, so we don't have an input manager.
    if (m_pInputManager == nullptr)
        return;

    Vector3f moveDir;

    if (m_pInputManager->m_bInputState[CONTROLS_FRONT])
        moveDir.z += 1.0f;
    if (m_pInputManager->m_bInputState[CONTROLS_BACK])
        moveDir.z -= 1.0f;
    if (m_pInputManager->m_bInputState[CONTROLS_LEFT])
        moveDir.x -= 1.0f;
    if (m_pInputManager->m_bInputState[CONTROLS_RIGHT])
        moveDir.x += 1.0f;

    if (m_pInputManager->m_bInputState[CONTROLS_UP])
        moveDir.y += 1.0f;
    if (m_pInputManager->m_bInputState[CONTROLS_DOWN])
        moveDir.y -= 1.0f;

    if (m_pInputManager->m_bInputState[CONTROLS_FLY])
        m_bFly = !m_bFly;

    float acceleration = 0.924f;

    moveDir *= 6.5f * 0.98f;

    m_velocity += moveDir * acceleration * dt;

    m_bGravity = !m_bFly;
}