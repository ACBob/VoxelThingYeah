#include "physics.hpp"

CBoundingBox::CBoundingBox( Vector3f vPosition, Vector3f size, Vector3f vOrigin )
{
	this->m_vPosition = vPosition;
	this->m_vBounds	  = size;
	this->m_vOrigin	  = vOrigin;
}

bool CBoundingBox::TestCollide( CBoundingBox other )
{
	// origin is 0-1 in each axis
	// if origin is 0,0,0 then m_vPosition is vMin
	// if origin is 1,1,1 then m_vPosition is vMax
	// so it follows that 0.5,0.5,0.5 is the center of the box

	Vector3f vMin = this->m_vPosition - this->m_vOrigin * this->m_vBounds;
	Vector3f vMax = this->m_vPosition + (VEC_ONE - this->m_vOrigin) * this->m_vBounds;

	Vector3f otherMin = other.m_vPosition - other.m_vOrigin * other.m_vBounds;
	Vector3f otherMax = other.m_vPosition + (VEC_ONE - other.m_vOrigin) * other.m_vBounds;

	return (vMin.x <= otherMax.x && vMax.x >= otherMin.x) &&
		   (vMin.y <= otherMax.y && vMax.y >= otherMin.y) &&
		   (vMin.z <= otherMax.z && vMax.z >= otherMin.z);
}

bool CBoundingBox::TestPointCollide( Vector3f pos )
{
	Vector3f vMin = this->m_vPosition - this->m_vOrigin * this->m_vBounds;
	Vector3f vMax = this->m_vPosition + (VEC_ONE - this->m_vOrigin) * this->m_vBounds;

	return (vMin.x <= pos.x && vMax.x >= pos.x) &&
		   (vMin.y <= pos.y && vMax.y >= pos.y) &&
		   (vMin.z <= pos.z && vMax.z >= pos.z);
}