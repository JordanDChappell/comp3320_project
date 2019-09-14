#version 400

in vec3 Color;
in vec2 TexCoord;
in float height;

out vec4 outColor;

uniform sampler2D texGrass;
uniform sampler2D texRock;
uniform sampler2D texWater;

void main()
{
	
	vec4 colTexture;
	if (height < 0.0) {
		colTexture = texture(texWater, TexCoord);
	}
	else if (height < 1.0) {
		colTexture = texture(texRock, TexCoord);
	}
	else {
		colTexture = texture(texGrass, TexCoord);
	}

	outColor = colTexture;
}