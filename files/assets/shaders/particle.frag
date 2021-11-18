#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Light;

#include "include/uniforms.glsl"

uniform sampler2D diffuse;

void main()
{
	vec2 texCoord = vec2(TexCoord.x, 1 - TexCoord.y) * texCoordOffset.xy + texCoordOffset.zw;
	FragColor = texture(diffuse, texCoord);
	if (FragColor.a == 0.0) discard;

	FragColor.rgb *= 0.5 * Light;
	FragColor.rgb *= (1 + lighting.rgb);
}