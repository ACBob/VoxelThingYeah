#include "player.h"
#include "SDL/SDL.h"

// TODO: seperate player from camera

Player::Player()
{

}

void Player::Update()
{
	if (inputMan->keyboardState['W'])
		pos = pos + Vector(1,0,0);
	
	MouseInput(inputMan->mouseMovement.x, inputMan->mouseMovement.y);

	// Rotate our forward vector towards pitch/yaw
	forward = Vector(0,0,1).Rotate(2, -yaw).Rotate(1, pitch);
}

void Player::MouseInput(float xrel, float yrel)
{
	pitch += yrel * 0.5;
	yaw += xrel * 0.5;

	pitch = pitch < -87 ? -87 : (pitch > 87 ? 87 : pitch);
}