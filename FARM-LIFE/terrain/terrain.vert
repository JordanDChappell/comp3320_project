#version 150

// in variables
in vec3 position;
in vec3 normal;
in vec2 texCoord;

// out variables
out vec2 TexCoord;
out float height;

// Transformation matrices
uniform mat4 Hwm;
uniform mat4 Hvw;
uniform mat4 Hcv;

// Other uniforms
uniform float scale;		// scale the terrain
uniform vec4 clippingPlane;	// clipping plane

void main()
{
	// Check if need to clip vertex based on clipping plane
	gl_ClipDistance[0] = dot(Hwm * vec4(position[0] * scale, position[1], position[2] * scale, 1.0), clippingPlane);
	TexCoord = texCoord;
	gl_Position = Hcv * Hvw * Hwm * vec4(position[0] * scale, position[1], position[2] * scale, 1.0);
	height = position[1];
}