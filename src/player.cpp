#include "player.h"
#include "SDL/SDL.h"

#include <iostream>

// TODO: seperate player from camera

Player::Player() :
	pos(0.0f, 0.0f, 5.0f),
	forward(0.0f, 0.0f, 1.0f)
{
	pitch = yaw = 0.0f;
	hand.length = 4;
	hand.dir = forward;
	hand.pos = pos;
}

void Player::Update(ChunkManager *chunkMan)
{
	Vector right = forward.Rotate(2, 90);
	right.y = 0;
	right = right.Normal();
	// TODO: Custom Controls (inputMan properties?)
	if (inputMan->keyboardState['W'])
		pos = pos + (forward * 0.2);
	else if(inputMan->keyboardState['S'])
		pos = pos + (forward * -0.2);
	if (inputMan->keyboardState['A'])
		pos = pos + (right * -0.2);
	else if(inputMan->keyboardState['D'])
		pos = pos + (right * 0.2);
	
	MouseInput(inputMan->mouseMovement.x, inputMan->mouseMovement.y);

	// Rotate our forward vector towards pitch/yaw
	forward = Vector(0,0,1).Rotate(1, pitch).Rotate(2, yaw).Normal();

	hand.pos = pos;
	hand.dir = forward;
	pointed = hand.Cast(chunkMan);
}

void Player::MouseInput(float xrel, float yrel)
{
	// TODO: sensitivity
	pitch += yrel * 0.1;
	yaw += xrel * 0.1;

	while (yaw > 360)
		yaw -= 360;
	while (yaw < 0)
		yaw += 360;

	pitch = pitch < -89.9 ? -89.9 : (pitch > 89.9 ? 89.9 : pitch);
}