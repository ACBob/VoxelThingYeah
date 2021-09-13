#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Light;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	mat4 billboard = model * view;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i==j)
				billboard[i*4][j] = 1.0f;
			else
				billboard[i*4][j] = 0.0f;
		}
	}

	gl_Position = projection * billboard * vec4(aPos, 0.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}