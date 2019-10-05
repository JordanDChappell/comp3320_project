#version 150
#define M_PI 3.1415926535897932384626433832795

// in variables
in vec2 TexCoord;
in vec2 normalTexCoord;				
in float height;					// height of the fragment
in vec3 toCameraVector;				// vector from vertex to camera
in vec3 fromLightVector;			// vector from vertex to light

// out variables
out vec4 outColor;					// colour of the fragment

// Transformation matrices
uniform mat4 Hwm;					// model to world transform
uniform mat4 Hvw;					// world to view transform
uniform mat4 Hcv;					// view to camera transform

// Textures
uniform sampler2D texGrass;			// grass texture
uniform sampler2D texRock;			// rock texture
uniform sampler2D texSand;			// sand texture
uniform sampler2D normalMap;		// normal map texture
uniform sampler2D waterNormalMap;	// normal map for underwater lighting

uniform float grassHeight;			// height value at which the terrain becomes grass
uniform float waterHeight;			// height of the water
uniform float time;					// time of the program
uniform vec3 lightColour;			// colour of the light

// Parameters for specular lighting
const float shineDamper = 20.0;		// adjust shine
const float reflectivity = 0.6;		// adjust reflectivity

void main()
{
	//-----------
	// GET NORMAL
	//-----------
	// Get normal from map for distorted texture coordinates to match the texture distortions
	vec4 normalMapColour = texture(normalMap, normalTexCoord);
	// Texture is only between 0-1, so adjust to get negative values in X-Z
	vec3 normal = vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b, normalMapColour.g * 2.0 - 1.0);
	normal = normalize(normal);

	float slope = acos(dot(normal, vec3(0, 1, 0)));

	//------------
	// GET TEXTURE
	//------------

	vec4 colTexture;
	vec4 grassTexture = texture(texGrass, TexCoord);
	vec4 rockTexture = texture(texRock, TexCoord);
	vec4 sandTexture = texture(texSand, TexCoord);

	// Texture will just be rock
	if (height < (grassHeight - 1)) {
		colTexture = sandTexture;
	}

	// Fade in the grass
	else if (height <= grassHeight && height >= (grassHeight - 1)) {
		colTexture = mix(sandTexture, grassTexture, height - (grassHeight - 1));
	}

	// Only grass
	else {
		colTexture = grassTexture;	
	} 

	// Add rocks to areas that slope a lot
	if (slope > M_PI/6) {
		colTexture = rockTexture;
	}

	// Under the water, add specular light from the water above
	if (height <= waterHeight) {
		// Get the water normal map texture
		vec4 waterNormalMapColour = texture(waterNormalMap, TexCoord/20 + time/25);

		// Texture is only between 0-1, so adjust to get negative values in X-Z
		vec3 waterNormal = vec3(waterNormalMapColour.r * 2.0 - 1.0, waterNormalMapColour.b, waterNormalMapColour.g * 2.0 - 1.0);
		waterNormal = normalize(waterNormal);

		// Find the specular light value based on the water normal		
		vec3 reflectedLight = reflect(normalize(fromLightVector), waterNormal);
		float specular = max(dot(reflectedLight, normalize(toCameraVector)), 0.0);
		specular = pow(specular, shineDamper);
		vec3 waterSpecularHighlights = lightColour * specular * reflectivity;

		// Add the specular highlights to the colour
		colTexture = colTexture + vec4(waterSpecularHighlights, 1.0);
	}

	outColor = colTexture;
}