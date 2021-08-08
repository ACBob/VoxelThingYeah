#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Color;

uniform sampler2D texture0;

void main()
{
	vec4 texelColor = texture(texture0, vec2(TexCoord.x, TexCoord.y));
	if (texelColor.a == 0.0) discard;
	FragColor = texelColor * vec4(Color.xyz, 1.0f);
}