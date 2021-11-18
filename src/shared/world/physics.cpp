#include "physics.hpp"

CBoundingBox::CBoundingBox( Vector3f vPosition, Vector3f size, Vector3f vOrigin )
{
	this->m_vPosition = vPosition;
	this->m_vBounds	  = size;
	this->m_vOrigin	  = vOrigin;
}

bool CBoundingBox::TestCollide( CBoundingBox other )
{
	// This is silly.
	// And unreadable because of the new variables, lovely!
	return ( ( other.m_vPosition.x - ( other.m_vBounds.x * other.m_vOrigin.x ) ) + other.m_vBounds.x >=
				 ( m_vPosition.x - ( m_vBounds.x * m_vOrigin.x ) ) &&
			 ( other.m_vPosition.x - ( other.m_vBounds.x * other.m_vOrigin.x ) ) <=
				 ( m_vPosition.x - ( m_vBounds.x * m_vOrigin.x ) + m_vBounds.x ) ) &&
		   ( ( other.m_vPosition.y - ( other.m_vBounds.y * other.m_vOrigin.y ) ) + other.m_vBounds.y >=
				 ( m_vPosition.y - ( m_vBounds.y * m_vOrigin.y ) ) &&
			 ( other.m_vPosition.y - ( other.m_vBounds.y * other.m_vOrigin.y ) ) <=
				 ( m_vPosition.y - ( m_vBounds.y * m_vOrigin.y ) + m_vBounds.y ) ) &&
		   ( ( other.m_vPosition.z - ( other.m_vBounds.z * other.m_vOrigin.z ) ) + other.m_vBounds.z >=
				 ( m_vPosition.z - ( m_vBounds.z * m_vOrigin.z ) ) &&
			 ( other.m_vPosition.z - ( other.m_vBounds.z * other.m_vOrigin.z ) ) <=
				 ( m_vPosition.z - ( m_vBounds.z * m_vOrigin.z ) + m_vBounds.z ) );
}

bool CBoundingBox::TestPointCollide( Vector3f pos )
{
	if ( pos < this->m_vPosition || pos > ( this->m_vPosition + this->m_vBounds ) )
		return false; // Out of m_vBounds

	// If it's in m_vBounds, it's logically going to collide
	return true;
}