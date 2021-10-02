#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Light;

#include "shaders/include/uniforms.glsl"

#define PI 3.1415926538

void main()
{
#	include "shaders/include/inworld.glsl"

	float f = 0.5 + 0.5 * cos(PI - PI * (timeOfDay / 12000.0)) * 0.75;
	f += 0.25 + 0.5 * f;
	Light = vec3(f);
}