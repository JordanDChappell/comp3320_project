#version 150

// in variables
in vec2 TexCoord;
in float height;

// out variables
out vec4 outColor;

// Textures
uniform sampler2D texGrass;	// grass texture
uniform sampler2D texRock;	// rock texture

// Height value at which the terrain becomes grass
uniform float grassHeight;

void main()
{
	vec4 colTexture;
	vec4 grassTexture = texture(texGrass, TexCoord);
	vec4 rockTexture = texture(texRock, TexCoord);

	// Texture will just be rock
	if (height < (grassHeight - 1)) {
		colTexture = rockTexture;
	}

	// Fade in the grass
	else if (height <= grassHeight && height >= (grassHeight - 1)) {
		colTexture = mix(rockTexture, grassTexture, height - (grassHeight - 1));
	}

	// Only grass
	else {
		colTexture = grassTexture;
	}

	outColor = colTexture;
}