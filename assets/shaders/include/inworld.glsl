// NOTE: This is to be included in the main of a shader
// Assumes uniforms.glsl is also included

gl_Position = projection * view * model * vec4(aPos, 1.0f);
TexCoord = vec2(aTexCoord.x, aTexCoord.y);