#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in float Light;

uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, vec2(TexCoord.x, 1 - TexCoord.y));
	FragColor.rgb *= vec3(Light, Light, Light);
}