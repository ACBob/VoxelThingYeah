#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out float Light;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int timeOfDay;
uniform vec3 sunAngle;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);

	Light = 0.3 + (min(max(dot(aNormal, sunAngle), 0.0), 0.7) * abs(1 - (timeOfDay / 12000.0)));
}