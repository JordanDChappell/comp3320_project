#version 150

// in variables
in vec3 position;					// position of the vertex

// out variables
out vec2 TexCoord;					// tex coordinates for tiling
out vec2 normalTexCoord;			// tex coordinates for the normals
out float height;					// height of the terrain at this vertex
out vec3 toCameraVector;			// vector from vertex to camera
//out vec3 fromLightVector;			// vector from vertex to light
out vec3 FragPos;					// Fragment Position

// Transformation matrices
uniform mat4 Hwm;					// model to world transform
uniform mat4 Hvw;					// world to view transform
uniform mat4 Hcv;					// view to clip transform

// Other uniforms
uniform float scale;				// scale the terrain
uniform vec4 clippingPlane;			// clipping plane
//uniform vec3 lightPosition;			// position of the light
uniform vec3 cameraPosition;		// camera position vector
uniform float resolutionX;			// resolution of terrain
uniform float resolutionZ;			// resolution of terrain

const float tilingFactor = 50.0;	// tiling factor

void main()
{
	// Check if need to clip vertex based on clipping plane
	gl_ClipDistance[0] = dot(Hwm * vec4(position[0] * scale, position[1], position[2] * scale, 1.0), clippingPlane);

	// Set position of the vertex
	vec4 worldPos = Hwm * vec4(position[0] * scale, position[1], position[2] * scale, 1.0);
	gl_Position = Hcv * Hvw * worldPos;

	height = position[1];
	FragPos = worldPos.xyz;

	// Find texture coordinates for both the normal (one texture across whole terrain) 
	// and tiling texture coordinates
	normalTexCoord = vec2(position[0]/resolutionX, position[2]/resolutionZ);
	TexCoord = vec2(position[0]/resolutionX, position[2]/resolutionZ) * tilingFactor;

	// Get vertex to camera vector
	toCameraVector = cameraPosition - worldPos.xyz;

	// Get vertex to light vector
	//fromLightVector = worldPos.xyz - lightPosition;
}
