#version 150

in vec3 position;
in vec3 color;
in vec2 texCoord;

out vec3 Color;
out vec2 TexCoord;

// Transformation matrix
uniform mat4 Hwm;
uniform mat4 Hvw;
uniform mat4 Hcv;

// Scale the terrain
uniform float scale;

void main()
{
	Color = color;
	TexCoord = texCoord;
    gl_Position = Hcv * Hvw * Hwm * vec4(position * scale, 1.0);

}