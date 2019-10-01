#version 150

in vec2 TexCoords;

out vec4 outColor;

uniform sampler2D screen;

void main()
{
    outColor = texture(screen, TexCoords);
}