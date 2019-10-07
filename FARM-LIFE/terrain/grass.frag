#version 330 core

in vec2 texCoords;

// out variables
out vec4 outColour;					// colour of the fragment

uniform sampler2D normalMap;

void main()
{
    outColour = texture(normalMap, texCoords);
}  