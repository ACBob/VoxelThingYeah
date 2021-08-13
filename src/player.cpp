#include "player.h"
#include "SDL/SDL.h"

#include <iostream>

Player::Player() :
	pos(0.0f, 16.0f, 0.0f),
	forward(0.0f, 0.0f, 1.0f),
	velocity(0.0f, 0.0f, 0.0f),
	collision(pos, Vector(0.5, 1.25, 0.5))
{
	pitch = yaw = 0.0f;
	hand.length = 4.25;
	hand.dir = forward;
	hand.pos = pos;
}

void Player::Update(ChunkManager *chunkMan, SoundManager *soundMan)
{
	Vector right = forward.Rotate(2, 90);
	right.y = 0;
	right = right.Normal();

	// HACK: since we reset it soon anyway....
	if (!noclipMode)
	{
		forward.y = 0;
		forward = forward.Normal();
	}
	// TODO: Custom Controls (inputMan properties?)
	if (inputMan->keyboardState['W'])
		velocity = velocity + (forward * 2.67);
	else if(inputMan->keyboardState['S'])
		velocity = velocity + (forward * -2.67);
	if (inputMan->keyboardState['A'])
		velocity = velocity + (right * -2.67);
	else if(inputMan->keyboardState['D'])
		velocity = velocity + (right * 2.67);

	if (onFloor && !noclipMode && inputMan->keyboardState[' '])
		velocity.y = 7.5;
	
	if (inputMan->keyboardState['V'] && !inputMan->oldKeyboardState['V'])
		noclipMode = !noclipMode;
	
	MouseInput(inputMan->mouseMovement.x, inputMan->mouseMovement.y);

	// Rotate our forward vector towards pitch/yaw
	forward = Vector(0,0,1).Rotate(1, pitch).Rotate(2, yaw).Normal();

	if (inputMan->mouseState & IN_LEFT_MOUSE && inputMan->oldMouseState == 0 && pointed.block != nullptr)
	{
		BlockFeatures bF = GetBlockFeatures(pointed.block->blockType);
		if (bF.breakable)
		{
			soundMan->PlayBreakSound(pointed.block->blockType, pointed.position - Vector(0.5, 0.5, 0.5));
			pointed.block->blockType = blocktype_t::AIR;
			pointed.block->Update();
		}
	}
	if (inputMan->mouseState & IN_RIGHT_MOUSE && inputMan->oldMouseState == 0 && pointed.block != nullptr)
	{
		Block *b = chunkMan->BlockAtWorldPos((pointed.position - 0.5) + pointed.normal);
		BlockFeatures bF = GetBlockFeatures(pointed.block->blockType);
		if (b != nullptr && bF.selectable)
		{
			blocktype_t oldType = b->blockType;
			b->blockType = selectedBlockType;
			if (!noclipMode && !chunkMan->TestAABBCollision(collision))
			{
				b->Update();
				soundMan->PlayPlaceSound(b->blockType, pointed.position + pointed.normal - Vector(0.5, 0.5, 0.5));
			}
			else if (noclipMode)
			{
				b->Update();
				soundMan->PlayPlaceSound(b->blockType, pointed.position + pointed.normal - Vector(0.5, 0.5, 0.5));
			}
			else
				b->blockType = oldType;
		}
	}

	if (inputMan->mouseState & IN_WHEEL_UP)
	{
		selectedBlockType = blocktype_t(selectedBlockType + 1);
		if (selectedBlockType == blocktype_t::BEDROCK)
			selectedBlockType = blocktype_t(selectedBlockType + 1);
		if (selectedBlockType > blocktype_t::GLASS)
			selectedBlockType = blocktype_t::STONE;
	}
	else if (inputMan->mouseState & IN_WHEEL_DOWN)
	{
		selectedBlockType = blocktype_t(selectedBlockType - 1);
		if (selectedBlockType == blocktype_t::BEDROCK)
			selectedBlockType = blocktype_t(selectedBlockType - 1);
		if (selectedBlockType <= blocktype_t::AIR)
			selectedBlockType = blocktype_t::GLASS;
	}

	camera.pos = pos + Vector(0,1,0);
	camera.forward = forward;

	hand.pos = camera.pos;
	hand.dir = camera.forward;
	pointed = hand.Cast(chunkMan);
}

void Player::UpdateCollision()
{
	collision.pos = pos;
}

void Player::Physics(double delta, ChunkManager *chunkMan)
{
	onFloor = false;

	delta *= delta;
	delta *= 60.0f;

	pos.x += velocity.x * delta;
	UpdateCollision();
	if (!noclipMode && chunkMan->TestAABBCollision(collision))
	{
		pos.x -= velocity.x * delta;
		velocity.x = 0;
	}
	pos.y += velocity.y * delta;
	UpdateCollision();
	if (!noclipMode && chunkMan->TestAABBCollision(collision))
	{
		// Don't glue ourselves to ceilings!
		if (velocity.y < 0) onFloor = true;
		pos.y -= velocity.y * delta;
		velocity.y = 0;
	}
	pos.z += velocity.z * delta;
	UpdateCollision();
	if (!noclipMode && chunkMan->TestAABBCollision(collision))
	{
		pos.z -= velocity.z * delta;
		velocity.z = 0;
	}
	
	if (!noclipMode)
	{
		velocity.x *= 0.3;
		velocity.z *= 0.3;
		if (onFloor)
		{
			velocity.x *= 0.95;
			velocity.z *= 0.95;
		}

		if (!onFloor)
			velocity.y += -(25)  * delta;
	}
	else
		velocity = velocity * 0.8;

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