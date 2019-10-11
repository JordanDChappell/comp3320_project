#version 330

// in variables
in vec3 position;					// position of the vertex

flat out vec2 positions; 

void main()
{
	gl_Position = vec4(position, 1.0);
	positions = position.xz;
}