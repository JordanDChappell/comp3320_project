#version 330
#define M_PI 3.1415926535897932384626433832795

// in type
layout (points) in;                                 // take in a point
layout (triangle_strip, max_vertices = 4) out;      // transform the point into a square 

out vec2 texCoords;

// Transformation matrices
uniform mat4 Hwm;					// model to world transform
uniform mat4 Hvw;					// world to view transform
uniform mat4 Hcv;					// view to clip transform

// Textures
uniform sampler2D normalMap;		// normal map texture for terrain, to determine slope

// Other uniforms
uniform float scale;				// scale the terrain
uniform float resX;                   // resolution of the terrain X
uniform float resZ;                   // resolution of the terrain Z
uniform float grassHeight;
uniform vec4 clippingPlane;			// clipping plane

// http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main()
{
    // Get position of the point
    vec4 pointPosition = gl_in[0].gl_Position;

    vec2 normalTexCoord = vec2(pointPosition[0] / resX, pointPosition[2] / resZ);
    // Get normal from map for distorted texture coordinates to match the texture distortions
	vec4 normalMapColour = texture(normalMap, normalTexCoord);
	// Texture is only between 0-1, so adjust to get negative values in X-Z
	vec3 normal = vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b, normalMapColour.g * 2.0 - 1.0);
	normal = normalize(normal);

    float angle = acos(dot(normal, vec3(0.0, 1.0, 0.0)));
    vec3 axis = cross(normal, vec3(0.0, 1.0, 0.0));

    mat4 rotate = rotationMatrix(axis, angle);
    
    // Check if need to clip vertex based on clipping plane
    float clip = dot(Hwm * vec4(pointPosition[0] * scale, pointPosition[1], pointPosition[2] * scale, 1.0), clippingPlane);

    if(clip < 0 || (pointPosition[1] < grassHeight || (abs(angle) > M_PI/6))) {
        gl_ClipDistance[0] = -1;
    }
    else {
        gl_ClipDistance[0] = 1;
    }
  
    texCoords = normalTexCoord;

    // CREATE SQUARE
    gl_Position = pointPosition + ( rotate * vec4(-0.5, 0.0, 0.0, 0.0));     // Bottom left
    gl_Position = Hcv * Hvw * Hwm * vec4(gl_Position.x * scale, gl_Position.y, gl_Position.z * scale, 1.0);
    EmitVertex();
    gl_Position = pointPosition + ( rotate * vec4(0.5, 0.0, 0.0, 0.0));     // Bottom right
    gl_Position = Hcv * Hvw * Hwm * vec4(gl_Position.x * scale, gl_Position.y, gl_Position.z * scale, 1.0);
    EmitVertex();
    gl_Position = pointPosition + ( rotate * vec4(-0.5, 1.0, 0.0, 0.0));     // Top left
    gl_Position = Hcv * Hvw * Hwm * vec4(gl_Position.x * scale, gl_Position.y, gl_Position.z * scale, 1.0);
    EmitVertex();
    gl_Position = pointPosition + ( rotate * vec4(0.5, 1.0, 0.0, 0.0) );     // Top right
    gl_Position = Hcv * Hvw * Hwm * vec4(gl_Position.x * scale, gl_Position.y, gl_Position.z * scale, 1.0);
    EmitVertex();
    EndPrimitive();
}