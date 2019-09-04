#version 150

in vec3 Color;
in vec2 TexCoord;

out vec4 outColor;

uniform sampler2D texGrass;

void main()
{
	vec4 colGrass = texture(texGrass, TexCoord);
	outColor = colGrass;
}