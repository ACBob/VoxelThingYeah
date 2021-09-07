#include "utility/vector.hpp"

#pragma once

class CCamera
{
  public:
	CVector m_position;

	// what direction the camera points
	CVector m_forward;
};