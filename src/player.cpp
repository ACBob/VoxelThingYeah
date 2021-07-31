#include "player.h"
#include "SDL/SDL.h"

Player::Player()
{

}

void Player::Input(bool e)
{
	dir = dir.Rotate(2, 5);
}