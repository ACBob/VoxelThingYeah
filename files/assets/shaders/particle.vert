#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Light;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 screen;
uniform mat3 normalMat;

uniform int timeOfDay;
uniform vec3 sunAngle;

uniform vec4 texCoordOffset;

uniform vec4 lighting;

#define PI 3.1415926538

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);

	float f = 0.5 + 0.5 * cos(PI - PI * (timeOfDay / 12000.0)) * 0.75;
	f += 0.25 + 0.5 * f;

	Light.r = Light.b = Light.g = f;
}