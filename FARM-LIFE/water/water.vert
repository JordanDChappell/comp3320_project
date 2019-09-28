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
uniform float time;
uniform float waveHeight;

void main()
{
	Colour = vec4(colour, 0.5);

	float yPosition = position[1] + ((cos(position[0] + (time * 2)) * 0.3) * cos((position[2] + (time * 2)) * 0.3) * waveHeight);

	gl_Position = Hcv * Hvw * Hwm * vec4(position[0] * scale, yPosition, position[2] * scale, 1.0);
}