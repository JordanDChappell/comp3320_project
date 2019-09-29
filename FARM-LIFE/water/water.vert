#version 150

// in variables
in vec3 position;				// position of the vertex

// out variables
out vec4 clipSpace;				// position of vertex in clip space
out vec2 texCoords;				// texture coordinates for du/dv and normal map
out vec3 toCameraVector;		// vector from vertex to camera
out vec3 fromLightVector;		// vector from vertex to light

// Transformation matrices
uniform mat4 Hwm;				// model to world matrix
uniform mat4 Hvw;				// world to view matrix
uniform mat4 Hcv;				// view to camera matrix

// Other uniforms
uniform vec3 cameraPosition;	// camera position vector
uniform float scale;			// scale the water (should match Terrain)
uniform float time;				// program time (to move the water)
uniform float waveHeight;		// height of the waves	
uniform vec3 lightPosition;		// position of the light

void main()
{
	// Transform the height to create waves
	float yPosition = position[1] + ((cos(position[0] + (time * 1.5)) * 0.15) * cos((position[2] + (time * 1.5)) * 0.15) * waveHeight) - waveHeight;

	// Get the world position of the vertex
	vec4 worldPos = Hwm * vec4(position[0] * scale, yPosition, position[2] * scale, 1.0);

	// Set position of vertex in clip space
	clipSpace = Hcv * Hvw * worldPos;

	// Set position of the vertex
	gl_Position = clipSpace;

	// Get texture coordinates based on position
	texCoords = vec2(position[0], position[2]) / 50;

	// Get vertex to camera vector
	toCameraVector = cameraPosition - worldPos.xyz;

	// Get vertex to light vector
	fromLightVector = worldPos.xyz - lightPosition;
}