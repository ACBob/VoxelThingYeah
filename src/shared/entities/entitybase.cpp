#include "entitybase.hpp"

EntityBase::EntityBase() :
	collisionBox({Vector(0), Vector(1,1,1)})
{

}

EntityBase::~EntityBase()
{

}

void EntityBase::Spawn()
{

}

void EntityBase::Kill()
{
	isKilled = true;
}