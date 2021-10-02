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

	Light = vec3(1.0f, 1.0f, 1.0f);
}