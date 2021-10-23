#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Light;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMat;

uniform int timeOfDay;
uniform vec3 sunAngle;

uniform int time;

#define PI 3.1415926538
#define WAVEINTENSITY 0.04

void main()
{
	float ftime = float(time) / 25.0;

	vec4 mPos = model * vec4(aPos, 1.0f);
	vec3 pos = aPos;
	pos.y -= 0.05;
	pos.y += sin(mPos.x + ftime) * WAVEINTENSITY;
	pos.y -= cos(mPos.z + ftime) * WAVEINTENSITY;

	pos.y += sin((mPos.x * mPos.z) + ftime/2.0) * WAVEINTENSITY*2;
	pos.y = min(pos.y, aPos.y);

	gl_Position = projection * view * model * vec4(pos, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);

	vec3 absSunAngle = sunAngle;
	absSunAngle.y = max(sunAngle.y, 0);

	vec3 Normal = normalMat * aNormal;

	float lightForTime = 0.5 + 0.5 * cos(PI - PI * (timeOfDay / 12000.0)) * 0.75;
	Light.x = 0.25 + (max(dot(Normal, absSunAngle) * 0.25, 0) * lightForTime) + (0.5 * lightForTime);
	Light.z = Light.y = Light.x;
}