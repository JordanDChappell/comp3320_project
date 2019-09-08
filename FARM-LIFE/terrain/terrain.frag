#version 150

in vec3 Color;
in vec2 TexCoord;
in float distanceCamera;

out vec4 outColor;

uniform sampler2D texGrass;

void main()
{
	vec4 colGrass = texture(texGrass, TexCoord);
	outColor = vec4(Color, 1.0);
}