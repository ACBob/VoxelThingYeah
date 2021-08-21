#include "entityplayer.h"

EntityPlayer::EntityPlayer()
{
	position = Vector(0,16,0);
#ifdef CLIENTEXE
	hand.length = 4;
	hand.dir = GetForward();
	hand.pos = position;
#endif
}
EntityPlayer::~EntityPlayer()
{
}

#ifdef CLIENTEXE
void EntityPlayer::UpdateClient(World *clientSideWorld)
{
	// Movement
	{
		// TODO: Custom Controls (inputMan properties?)
		Vector forward = GetForward();
		Vector right = forward.Rotate(2, 90);
		if (inputMan->keyboardState['W'])
			velocity = velocity + (forward * 2.67);
		else if(inputMan->keyboardState['S'])
			velocity = velocity + (forward * -2.67);
		if (inputMan->keyboardState['A'])
			velocity = velocity + (right * -2.67);
		else if(inputMan->keyboardState['D'])
			velocity = velocity + (right * 2.67);

		if (onFloor && inputMan->keyboardState[' '])
			velocity.y = 7.5;
	}
	
	// Mouse Input
	{
		// TODO: sensitivity
		float pitch = rotation.x;
		float yaw = rotation.y;

		pitch -= inputMan->mouseMovement.y * 0.1;
		yaw += inputMan->mouseMovement.x * 0.1;

		while (yaw > 360)
			yaw -= 360;
		while (yaw < 0)
			yaw += 360;

		pitch = pitch < -89.9 ? -89.9 : (pitch > 89.9 ? 89.9 : pitch);

		rotation.x = pitch;
		rotation.y = yaw;
	}
	
	camera.forward = GetForward();
	camera.pos = position + Vector(0, 1.6, 0);

	hand.pos = camera.pos;
	hand.dir = camera.forward;
	pointed = hand.Cast(clientSideWorld);

	if (inputMan->mouseState & IN_LEFT_MOUSE && inputMan->oldMouseState == 0 && pointed.block != nullptr)
	{
		BlockFeatures bF = GetBlockFeatures(pointed.block->blockType);
		if (bF.breakable)
		{
			// soundMan->PlayBreakSound(pointed.block->blockType, pointed.position - Vector(0.5, 0.5, 0.5));
			pointed.block->blockType = blocktype_t::AIR;
			pointed.block->Update();
		}
	}
	if (inputMan->mouseState & IN_RIGHT_MOUSE && inputMan->oldMouseState == 0 && pointed.block != nullptr)
	{
		Block *b = clientSideWorld->BlockAtWorldPos((pointed.position - 0.5) + pointed.normal);
		BlockFeatures bF = GetBlockFeatures(pointed.block->blockType);
		if (b != nullptr && bF.selectable)
		{
			blocktype_t oldType = b->blockType;
			b->blockType = selectedBlockType;
			if (!clientSideWorld->TestAABBCollision(collisionBox))
			{
				b->Update();
				// soundMan->PlayPlaceSound(b->blockType, pointed.position + pointed.normal - Vector(0.5, 0.5, 0.5));
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
		if (selectedBlockType > blocktype_t::WATER)
			selectedBlockType = blocktype_t::STONE;
	}
	else if (inputMan->mouseState & IN_WHEEL_DOWN)
	{
		selectedBlockType = blocktype_t(selectedBlockType - 1);
		if (selectedBlockType == blocktype_t::BEDROCK)
			selectedBlockType = blocktype_t(selectedBlockType - 1);
		if (selectedBlockType <= blocktype_t::AIR)
			selectedBlockType = blocktype_t::WATER;
	}
}
#endif

void EntityPlayer::Tick()
{
}

#ifdef SERVEREXE
void EntityPlayer::PhysicsTick(float delta, World *world)
{
	UpdateCollision();
	if (world->TestAABBCollision(collisionBox))
	{
		position.x -= velocity.x * delta;
		velocity.x = 0;
	}
	position.y += velocity.y * delta;
	UpdateCollision();
	if (world->TestAABBCollision(collisionBox))
	{
		// Don't glue ourselves to ceilings!
		if (velocity.y < 0) onFloor = true;
		position.y -= velocity.y * delta;
		velocity.y = 0;
	}
	position.z += velocity.z * delta;
	UpdateCollision();
	if (world->TestAABBCollision(collisionBox))
	{
		position.z -= velocity.z * delta;
		velocity.z = 0;
	}

	velocity = velocity * 0.8;
}
#endif