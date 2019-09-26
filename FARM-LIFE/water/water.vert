#version 150

in vec3 position;

out vec4 Colour;

// Transformation matrix
uniform mat4 Hwm;
uniform mat4 Hvw;
uniform mat4 Hcv;

// Scale the water
uniform float scale;
// Colour of the water
uniform vec3 colour;

void main()
{
	Colour = vec4(colour, 0.5);
	gl_Position = Hcv * Hvw * Hwm * vec4(position[0] * scale, position[1], position[2] * scale, 1.0);
}