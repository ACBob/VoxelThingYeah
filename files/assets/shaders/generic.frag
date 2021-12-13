#version 450 core
out vec4 FragColor;

in vec2 TexCoord;

#include "include/uniforms.glsl"

uniform sampler2D diffuse;

void main()
{
	vec2 texCoord = vec2(TexCoord.x, 1 - TexCoord.y);
	FragColor = texture(diffuse, texCoord);
	if (FragColor.a == 0.0) discard;
}