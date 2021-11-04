#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 SunLight;
in vec4 AmbientLight;
in vec3 VertexColour;

uniform sampler2D texture1;

#define FOG_DENSITY 3
#define FOG_START_DISTANCE 0.99
#define FOG_END_DISTANCE 1.0
#define FOG_COLOUR vec3(0.5, 0.5, 0.5)

void main()
{
	FragColor = texture(texture1, vec2(TexCoord.x, 1 - TexCoord.y));
	if (FragColor.a == 0.0) discard;

	FragColor.rgb *= VertexColour;
	FragColor.rgb *= 0.75 * SunLight;
	FragColor.rgb *= (1 + AmbientLight.rgb);

	// Screen-space fog
	float fogFactor = clamp((FOG_END_DISTANCE - gl_FragCoord.z) / (FOG_END_DISTANCE - FOG_START_DISTANCE), 0.0, 1.0);
	fogFactor = pow(fogFactor, 1.0 / FOG_DENSITY);
	FragColor.rgb = mix(FOG_COLOUR, FragColor.rgb, fogFactor);
}