#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Light;

#include "shaders/include/uniforms.glsl"

uniform int time;

#define PI 3.1415926538
#define WAVEINTENSITY 0.04

void main()
{
	float ftime = float(time) / 25.0;

	vec4 mPos = model * vec4(aPos, 1.0f);
	vec3 pos = aPos;
	pos.y -= 0.05;
	pos.y += sin(mPos.x + ftime) * WAVEINTENSITY;
	pos.y -= cos(mPos.z + ftime) * WAVEINTENSITY;

	pos.y += sin((mPos.x * mPos.z) + ftime/2.0) * WAVEINTENSITY*2;
	pos.y = min(pos.y, aPos.y);

#	include "shaders/include/inworld.glsl"
#	include "shaders/include/diffuse.glsl"
}