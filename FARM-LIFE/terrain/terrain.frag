#version 150
#define M_PI 3.1415926535897932384626433832795

// in variables
in vec2 TexCoord;
in vec2 normalTexCoord;
in float height;

// out variables
out vec4 outColor;

// Textures
uniform sampler2D texGrass;		// grass texture
uniform sampler2D texRock;		// rock texture
uniform sampler2D texSand;		// sand texture
uniform sampler2D normalMap;	// normal map texture

// Height value at which the terrain becomes grass
uniform float grassHeight;

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

	if(slope > M_PI/6) {
		colTexture = rockTexture;
	}

	outColor = colTexture;
}