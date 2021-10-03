// Built-in error shaders for when we can't load any
// Used in dire situations (or if the user deleted something)

const char *const ERRORVERT = "#version 450 core \n \ 
layout (location = 0) in vec3 aPos; \n \ 
layout (location = 1) in vec3 aNormal; \n \ 
layout (location = 2) in vec2 aTexCoord; \n \ 
out vec2 TexCoord; \n \ 
out vec3 Light; \n \ 
uniform mat4 model; \n \ 
uniform mat4 view; \n \ 
uniform mat4 projection; \n \ 
void main() \n \ 
{ \n \ 
	gl_Position = projection * view * model * vec4(aPos, 1.0f); \n \ 
	TexCoord = vec2(aTexCoord.x, aTexCoord.y); \n \ 
}";

const char *const ERRORFRAG = "#version 450 core \n \ 
out vec4 FragColor; \n \ 
in vec2 TexCoord; \n \ 
uniform sampler2D diffuse; \n \ 
uniform int time; \n \ 
void main() \n \ 
{ \n \ 
	float fTime = time / 7.0; \n \ 
	float f = max(min(sin(fTime) + 0.5, 1.0), 0.5); \n \ 
	FragColor = texture(diffuse, vec2(TexCoord.x, 1 - TexCoord.y)); \n \ 
	FragColor.rgb *= f; \n \ 
}";