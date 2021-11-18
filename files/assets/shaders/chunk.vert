#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aVertexColour;
layout (location = 4) in vec4 aLighting;

out vec2 TexCoord;
out vec3 SunLight;
out vec4 AmbientLight;
out vec3 VertexColour;

#include "include/uniforms.glsl"

#define PI 3.1415926538

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);

	vec3 absSunAngle = sunAngle;
	absSunAngle.y = max(sunAngle.y, 0);

	vec3 Normal = normalMat * aNormal;

	float lightForTime = 0.5 + 0.5 * cos(PI - PI * (timeOfDay / 12000.0)) * 0.75;
	SunLight.r = SunLight.g = SunLight.b = 0.25 + (max(dot(Normal, absSunAngle) * 0.25, 0) * lightForTime) + (0.5 * lightForTime);

	AmbientLight = aLighting;
	VertexColour = aVertexColour;
}