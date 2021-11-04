#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Light;

#include "include/uniforms.glsl"

uniform sampler2D diffuse;

#define FOG_DENSITY 3
#define FOG_START_DISTANCE 0.99
#define FOG_END_DISTANCE 1.0
#define FOG_COLOUR vec3(0.5, 0.5, 0.5)

void main()
{
	vec2 texCoord = vec2(TexCoord.x, TexCoord.y) * texCoordOffset.xy + texCoordOffset.zw;
	texCoord.y = 1 - texCoord.y;
	FragColor = texture(diffuse, texCoord);
	if (FragColor.a == 0.0) discard;

	FragColor.rgb *= Light * 0.5;
	FragColor.rgb *= (1 + lighting.rgb);
	
	// Screen-space fog
	float fogFactor = clamp((FOG_END_DISTANCE - gl_FragCoord.z) / (FOG_END_DISTANCE - FOG_START_DISTANCE), 0.0, 1.0);
	fogFactor = pow(fogFactor, 1.0 / FOG_DENSITY);
	FragColor.rgb = mix(FOG_COLOUR, FragColor.rgb, fogFactor);
}