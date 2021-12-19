#include "entityplayer.hpp"

CEntityPlayer::CEntityPlayer(entityId_t id) : BaseClass(id)
{
}

CEntityPlayer::~CEntityPlayer()
{
}

void CEntityPlayer::Update(float dt)
{
    BaseClass::Update(dt);
}