#version 150

in vec3 position;
in vec3 color;
in vec2 texCoord;

out vec3 Color;
out vec2 TexCoord;
out float distanceCamera;

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
	mat4 Hcm = Hcv * Hvw * Hwm;
    gl_Position = Hcm * vec4(position[0] * scale, position[1], position[2] * scale, 1.0);
	distanceCamera = Hcm[3].length();
}