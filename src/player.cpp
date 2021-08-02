#include "player.h"
#include "SDL/SDL.h"

#include <iostream>

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
	forward = Vector(0,0,1).Rotate(1, pitch).Rotate(2, yaw).Normal();
}

void Player::MouseInput(float xrel, float yrel)
{
	pitch -= yrel;
	yaw -= xrel;

	while (yaw > 360)
		yaw -= 360;
	while (yaw < 0)
		yaw += 360;

	pitch = pitch < -87 ? -87 : (pitch > 87 ? 87 : pitch);
}