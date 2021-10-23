#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 SunLight;
in vec4 AmbientLight;
in vec3 VertexColour;

uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, vec2(TexCoord.x, 1 - TexCoord.y));
	if (FragColor.a == 0.0) discard;

	FragColor.rgb *= VertexColour;
	FragColor.rgb *= 0.75 * SunLight;
	FragColor.rgb *= (1 + AmbientLight.rgb);
}