#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out float Light;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * vec4(aPos, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);

	Light = min(max(dot(aNormal, vec3(0,1,0)), 0.2) + (max(dot(aNormal, vec3(1,0,0)), 0.2) * 0.34), 1);
}