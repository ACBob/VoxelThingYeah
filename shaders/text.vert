#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;

out vec2 TexCoord;
out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 screen;

void main()
{
	gl_Position = screen * vec4(aPos.xy, 0.0, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	Color = aColor;
}