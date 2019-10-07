#version 330

// in variables
in vec3 position;					// position of the vertex

void main()
{
	gl_Position = vec4(position, 1.0);
}