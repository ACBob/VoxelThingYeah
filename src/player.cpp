#include "player.h"
#include "SDL/SDL.h"

// TODO: seperate player from camera

Player::Player()
{

}

void Player::Update()
{
	// TODO: Custom Controls (inputMan properties?)
	if (inputMan->keyboardState['W'])
		pos = pos + (forward.Normal() * -0.2);
	else if(inputMan->keyboardState['S'])
		pos = pos + (forward.Normal() * 0.2);
	if (inputMan->keyboardState['A'])
		pos = pos + (forward.Normal().Rotate(2, 90) * -0.2);
	else if (inputMan->keyboardState['D'])
		pos = pos + (forward.Normal().Rotate(2, 90) * 0.2);
	
	MouseInput(inputMan->mouseMovement.x, inputMan->mouseMovement.y);

	// Rotate our forward vector towards pitch/yaw
	forward = Vector(0,0,1).Rotate(2, -yaw).Rotate(1, pitch);
}

void Player::MouseInput(float xrel, float yrel)
{
	pitch += yrel;
	yaw += xrel;

	pitch = pitch < -87 ? -87 : (pitch > 87 ? 87 : pitch);
}