#version 150

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec2 TexCoord;
out float height;

// Transformation matrix
uniform mat4 Hwm;
uniform mat4 Hvw;
uniform mat4 Hcv;

// Scale the terrain
uniform float scale;

// Specify clipping plane
uniform vec4 clippingPlane;

void main()
{
	gl_ClipDistance[0] = dot(Hwm * vec4(position[0] * scale, position[1], position[2] * scale, 1.0), clippingPlane);
	TexCoord = texCoord;
	gl_Position = Hcv * Hvw * Hwm * vec4(position[0] * scale, position[1], position[2] * scale, 1.0);
	height = position[1];
}