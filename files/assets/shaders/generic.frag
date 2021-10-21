#version 450 core
out vec4 FragColor;

in vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 screen;
uniform mat3 normalMat;

uniform int timeOfDay;
uniform vec3 sunAngle;

uniform vec4 texCoordOffset;

uniform vec4 lighting;

uniform sampler2D diffuse;

void main()
{
	vec2 texCoord = vec2(TexCoord.x, TexCoord.y) * texCoordOffset.xy + texCoordOffset.zw;
	texCoord.y = 1 - texCoord.y;
	FragColor = texture(diffuse, texCoord);
	if (FragColor.a == 0.0) discard;

	FragColor.rgb *= 0.5;
	FragColor.rgb *= (1 + lighting.rgb);
}