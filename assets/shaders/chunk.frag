#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in vec4 Light;

#include "shaders/include/uniforms.glsl"

uniform sampler2D diffuse;

void main()
{
	vec2 texCoord = vec2(TexCoord.x, TexCoord.y) * texCoordOffset.xy + texCoordOffset.zw;
	texCoord.y = 1 - texCoord.y;
	FragColor = texture(diffuse, texCoord);
	if (FragColor.a == 0.0) discard;

	FragColor.rgb *= 0.5;
	FragColor.rgb *= (1 + Light.rgb);
}