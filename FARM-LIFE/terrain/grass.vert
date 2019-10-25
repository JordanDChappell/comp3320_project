#version 330

// in variables
in vec3 position;					// position of the vertex


flat out vec2 positions; 

uniform mat4 Hwm;					// model to world transform
uniform mat4 Hvw;					// world to view transform
uniform mat4 Hcv;					// view to clip transform
uniform float scale;				// scale the terrain
// Textures
uniform sampler2D normalMap;		// normal map texture for terrain, to determine slope
uniform float resX;                   // resolution of the terrain X
uniform float resZ; 

void main()
{
	gl_Position = vec4(position, 1.0);
	positions = position.xz;
}
