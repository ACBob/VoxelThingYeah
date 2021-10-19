#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aLighting;

out vec2 TexCoord;
out vec4 Light;

#include "include/uniforms.glsl"

#define PI 3.1415926538

void main()
{
#	include "include/inworld.glsl"
	Light = aLighting;
}