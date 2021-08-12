#include "player.h"
#include "SDL/SDL.h"

#include <iostream>

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

void Player::Update(ChunkManager *chunkMan, SoundManager *soundMan, double delta)
{
	Vector right = forward.Rotate(2, 90);
	right.y = 0;
	right = right.Normal();

	// HACK: since we reset it soon anyway....
	forward.y = 0;
	forward = forward.Normal();
	// TODO: Custom Controls (inputMan properties?)
	if (inputMan->keyboardState['W'])
		velocity = velocity + (forward * 0.05);
	else if(inputMan->keyboardState['S'])
		velocity = velocity + (forward * -0.05);
	if (inputMan->keyboardState['A'])
		velocity = velocity + (right * -0.05);
	else if(inputMan->keyboardState['D'])
		velocity = velocity + (right * 0.05);
	
	velocity.y += -9.8 * delta;

	if (onFloor && inputMan->keyboardState[' '])
		velocity.y = 0.24;
	
	MouseInput(inputMan->mouseMovement.x, inputMan->mouseMovement.y);

	// Rotate our forward vector towards pitch/yaw
	forward = Vector(0,0,1).Rotate(1, pitch).Rotate(2, yaw).Normal();

	if (inputMan->mouseState & IN_LEFT_MOUSE && inputMan->oldMouseState == 0 && pointed.block != nullptr && pointed.block->blockType != blocktype_t::BEDROCK)
	{
		soundMan->PlayBreakSound(pointed.block->blockType, pointed.position - Vector(0.5, 0.5, 0.5));
		pointed.block->blockType = blocktype_t::AIR;
		pointed.block->Update();
	}
	if (inputMan->mouseState & IN_RIGHT_MOUSE && inputMan->oldMouseState == 0 && pointed.block != nullptr && pointed.block->blockType != blocktype_t::AIR)
	{
		Block *b = chunkMan->BlockAtWorldPos((pointed.position - 0.5) + pointed.normal);
		if (b != nullptr && b->blockType == blocktype_t::AIR)
		{
			blocktype_t oldType = b->blockType;
			b->blockType = selectedBlockType;
			if (!chunkMan->TestAABBCollision(pos - Vector(0.25, 1, 0.25), Vector(0.5, 0.9, 0.5)))
			{
				b->Update();
				soundMan->PlayPlaceSound(b->blockType, pointed.position - Vector(0.5, 0.5, 0.5));
			}
			else
				b->blockType = oldType;
		}
	}

	if (inputMan->mouseState & IN_WHEEL_UP)
	{
		selectedBlockType = blocktype_t(selectedBlockType + 1);
		if (selectedBlockType > blocktype_t::PLANKS)
			selectedBlockType = blocktype_t::STONE;
	}
	else if (inputMan->mouseState & IN_WHEEL_DOWN)
	{
		selectedBlockType = blocktype_t(selectedBlockType - 1);
		if (selectedBlockType <= blocktype_t::AIR)
			selectedBlockType = blocktype_t::PLANKS;
	}

	onFloor = false;

	pos.x += velocity.x;
	if (chunkMan->TestAABBCollision(pos - Vector(0.25, 1, 0.25), Vector(0.5, 0.9, 0.5)))
	{
		pos.x -= velocity.x;
		velocity.x = 0;
	}
	pos.y += velocity.y;
	if (chunkMan->TestAABBCollision(pos - Vector(0.25, 1, 0.25), Vector(0.5, 0.9, 0.5)))
	{
		pos.y -= velocity.y;
		velocity.y = 0;
		onFloor = true;
	}
	pos.z += velocity.z;
	if (chunkMan->TestAABBCollision(pos - Vector(0.25, 1, 0.25), Vector(0.5, 0.9, 0.5)))
	{
		pos.z -= velocity.z;
		velocity.z = 0;
	}

	velocity.x *= 0.5;
	velocity.z *= 0.5;

	camera.pos = pos + Vector(0,0.65,0);
	camera.forward = forward;

	hand.pos = camera.pos;
	hand.dir = camera.forward;
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