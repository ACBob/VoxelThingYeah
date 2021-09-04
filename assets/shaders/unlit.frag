#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Light;

uniform sampler2D diffuse;

void main()
{
	FragColor = texture(diffuse, vec2(TexCoord.x, 1 - TexCoord.y));
	if (FragColor.a == 0.0) discard;
}