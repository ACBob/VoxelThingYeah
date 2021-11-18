// Matrices
uniform mat4 model; // Model Matrix
uniform mat4 view; // View Matrix
uniform mat4 projection; // Projection Matrix
uniform mat4 screen; // Screen matrix, where it is 0, screenX & 0, screenY
uniform mat3 normalMat; // Normal Matrix

uniform int timeOfDay; // The time of the current day, in ticks
uniform vec3 sunAngle; // The angle of the sun, as a unit vector

uniform vec4 texCoordOffset; // The offset to apply to the UV Coords, where xy is the scale and zw is the position

uniform vec4 lighting; // The lighting to apply to the vertices universally, on-top of any vertex-builtin ones.

uniform int time; // The time of the game, since the start of it