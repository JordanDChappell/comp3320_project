#version 150

in vec2 TexCoord;
in float height;

out vec4 outColor;

uniform sampler2D texGrass;
uniform sampler2D texRock;
uniform sampler2D texWater;

void main()
{
	vec4 colTexture;

	// Water is a mixture of water texture and rock texture
	if (height < 6.0) {
		colTexture = mix(texture(texWater, TexCoord), texture(texRock, TexCoord), 0.3);
	}

	// Fade out the water texture over the rock
	else if (height <= 7.0 && height >= 6.0) {
		colTexture = mix(texture(texWater, TexCoord), texture(texRock, TexCoord), ( ( (height - 6.0) * 0.7 ) / 1.0  ) + 0.3);
	}

	// Texture will just be rock
	else if (height <= 8.0 && height >= 7.0) {
		colTexture = texture(texRock, TexCoord);
	}

	// Fade in the grass
	else if (height <= 9.0 && height >= 8.0) {
		colTexture = mix(texture(texRock, TexCoord), texture(texGrass, TexCoord), ( ( (height - 8.0) * 1.0 ) / 1.0  ) + 0.0);
	}

	// Only grass
	else {
		colTexture = texture(texGrass, TexCoord);
	}
	outColor = colTexture;
}