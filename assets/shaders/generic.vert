#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Light;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int timeOfDay;
uniform vec3 sunAngle;

#define PI 3.1415926538

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);

	vec3 absSunAngle = sunAngle;
	absSunAngle.y = max(sunAngle.y, 0);

	float lightForTime = 0.5 - 0.5 * cos(PI - PI * (timeOfDay / 12000.0)) * 0.75;
	Light.x = 0.25 + (max(dot(aNormal, absSunAngle) * 0.25, 0) * lightForTime) + (0.5 * lightForTime);
	Light.z = Light.y = Light.x;
}