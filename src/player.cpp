#include "player.h"
#include "SDL/SDL.h"

#include <iostream>

// TODO: seperate player from camera

Player::Player() :
	pos(0.0f, 16.0f, 0.0f),
	forward(0.0f, 0.0f, 1.0f),
	velocity(0.0f, 0.0f, 0.0f)
{
	pitch = yaw = 0.0f;
	hand.length = 4.123;
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
		velocity = velocity + (forward * 0.2);
	else if(inputMan->keyboardState['S'])
		velocity = velocity + (forward * -0.2);
	if (inputMan->keyboardState['A'])
		velocity = velocity + (right * -0.2);
	else if(inputMan->keyboardState['D'])
		velocity = velocity + (right * 0.2);
	
	MouseInput(inputMan->mouseMovement.x, inputMan->mouseMovement.y);

	// Rotate our forward vector towards pitch/yaw
	forward = Vector(0,0,1).Rotate(1, pitch).Rotate(2, yaw).Normal();

	hand.pos = pos;
	hand.dir = forward;
	pointed = hand.Cast(chunkMan);

	if (inputMan->mouseState & IN_LEFT_MOUSE && inputMan->oldMouseState == 0 && pointed.block != nullptr && pointed.block->blockType != blocktype_t::BEDROCK)
	{
		pointed.block->blockType = blocktype_t::AIR;
		pointed.block->Update();
	}
	if (inputMan->mouseState & IN_RIGHT_MOUSE && inputMan->oldMouseState == 0 && pointed.block != nullptr && pointed.block->blockType != blocktype_t::AIR)
	{
		Block *b = chunkMan->BlockAtWorldPos((pointed.position - 0.5) + pointed.normal);
		if (b != nullptr && b->blockType == blocktype_t::AIR)
		{
			b->blockType = selectedBlockType;
			b->Update();
		}
	}

	if (inputMan->mouseState & IN_WHEEL_UP)
	{
		selectedBlockType = blocktype_t(selectedBlockType + 1);
		if (selectedBlockType > blocktype_t::PLANKS)
			selectedBlockType = blocktype_t::PLANKS;
	}
	else if (inputMan->mouseState & IN_WHEEL_DOWN)
	{
		selectedBlockType = blocktype_t(selectedBlockType - 1);
		if (selectedBlockType <= blocktype_t::AIR)
			selectedBlockType = blocktype_t::PLANKS;
	}

	pos.x += velocity.x;
	if (chunkMan->TestPointCollision(pos))
		pos.x -= velocity.x;
	pos.y += velocity.y;
	if (chunkMan->TestPointCollision(pos))
		pos.y -= velocity.y;
	pos.z += velocity.z;
	if (chunkMan->TestPointCollision(pos))
		pos.z -= velocity.z;
	
	velocity = Vector(0,0,0);
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