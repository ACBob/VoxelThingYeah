#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

#include "include/uniforms.glsl"

out vec3 vPosition;
out vec3 vSunAngle;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	vPosition = aPos;
	
	vSunAngle = sunAngle;
}