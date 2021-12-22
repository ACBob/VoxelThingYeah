#include "entityplayer.hpp"

CEntityPlayer::CEntityPlayer(entityId_t id, CWorld *pWorld) : BaseClass(id, pWorld)
{
}

CEntityPlayer::~CEntityPlayer()
{
}

void CEntityPlayer::Update(float dt)
{
    BaseClass::Update(dt);
}