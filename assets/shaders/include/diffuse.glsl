// NOTE: This is to be included in the main of a shader
// Assumes uniforms.glsl is also included

vec3 absSunAngle = sunAngle;
absSunAngle.y = max(sunAngle.y, 0);

vec3 Normal = normalMat * aNormal;

float lightForTime = 0.5 + 0.5 * cos(PI - PI * (timeOfDay / 12000.0)) * 0.75;
Light.x = 0.25 + (max(dot(Normal, absSunAngle) * 0.25, 0) * lightForTime) + (0.5 * lightForTime);
Light.z = Light.y = Light.x;