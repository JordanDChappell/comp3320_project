#version 150

in vec2 TexCoord;
in float height;

out vec4 outColor;

// Textures of the grass and the rock
uniform sampler2D texGrass;
uniform sampler2D texRock;

// Height value at which the terrain becomes grass
uniform float grassHeight;

void main()
{
	vec4 colTexture;

	// Texture will just be rock
	if (height < (grassHeight - 1)) {
		colTexture = texture(texRock, TexCoord);
	}

	// Fade in the grass
	else if (height <= grassHeight && height >= (grassHeight - 1)) {
		colTexture = mix(texture(texRock, TexCoord), texture(texGrass, TexCoord), height - (grassHeight - 1));
	}

	// Only grass
	else {
		colTexture = texture(texGrass, TexCoord);
	}

	outColor = colTexture;
}