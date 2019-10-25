#version 330

// in variables
in vec3 position;					// position of the vertex


flat out vec2 positions; 
//out vec3 FragPos;
out vec3 toCameraVector;
out vec3 norm;

uniform mat4 Hwm;					// model to world transform
uniform mat4 Hvw;					// world to view transform
uniform mat4 Hcv;					// view to clip transform
uniform float scale;				// scale the terrain
uniform vec3 cameraPosition;		// camera position vector
// Textures
uniform sampler2D normalMap;		// normal map texture for terrain, to determine slope
uniform float resX;                   // resolution of the terrain X
uniform float resZ; 

void main()
{
	gl_Position = vec4(position, 1.0);
	positions = position.xz;

	//experimental

	// Set position of the vertex
	
	FragPos = (Hwm * vec4(position, 1.0)).xyz;
	
	//end of exp


	//FragPos = vec3(position.x, position.y, position.z);

	// Get vertex to camera vector
	toCameraVector = cameraPosition - position.xyz;


	 vec4 pointPosition =  vec4(position, 1.0);
	// GET THE NORMAL VECTOR OF THE POINT ON THE TERRAIN TO ALIGN THE
    // GRASS CARD CORRECTLY
    vec2 normalTexCoord = vec2(pointPosition[0] / resX, pointPosition[2] / resZ);
    // Get normal from map for distorted texture coordinates to match the texture distortions
	vec4 normalMapColour = texture(normalMap, normalTexCoord);
	// Texture is only between 0-1, so adjust to get negative values in X-Z
	vec3 normal = vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b, normalMapColour.g * 2.0 - 1.0);
	normal = normalize(normal);

	// Output for the normal
	norm = normal;

}
