#version 150

in vec3 position;

out vec4 clipSpace;
out vec2 texCoords;
out vec3 toCameraVector;
out vec3 fromLightVector;

// Transformation matrix
uniform mat4 Hwm;
uniform mat4 Hvw;
uniform mat4 Hcv;
uniform vec3 cameraPosition;

// Scale the water
uniform float scale;
// Colour of the water
uniform float time;
// Height of the waves
uniform float waveHeight;

uniform vec3 lightPosition;

void main()
{
	float yPosition = position[1] + ((cos(position[0] + (time * 1.5)) * 0.15) * cos((position[2] + (time * 1.5)) * 0.15) * waveHeight) - waveHeight;

	vec4 worldPos = Hwm * vec4(position[0] * scale, yPosition, position[2] * scale, 1.0);

	clipSpace = Hcv * Hvw * worldPos;

	gl_Position = clipSpace;

	texCoords = vec2(position[0], position[2]) / 50;

	toCameraVector = cameraPosition - worldPos.xyz;

	fromLightVector = worldPos.xyz - lightPosition;
}