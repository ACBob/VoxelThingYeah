#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Light;

#include "include/uniforms.glsl"

uniform sampler2D diffuse;

void main()
{
	vec2 texCoord = vec2(TexCoord.x, TexCoord.y) * texCoordOffset.xy + texCoordOffset.zw;
	texCoord.y = 1 - texCoord.y;
	FragColor = texture(diffuse, texCoord);
	if (FragColor.a == 0.0) discard;

	FragColor.rgb *= Light * 0.5;
	FragColor.rgb *= (1 + lighting.rgb);
}