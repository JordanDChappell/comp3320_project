#version 330
#define M_PI 3.1415926535897932384626433832795

// in type
layout (points) in;                                 // take in a point
layout (triangle_strip, max_vertices = 12) out;      // transform the point into a square 

flat in vec2 positions[];

out vec2 texCoords;
flat out int grassType;

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
uniform float grassScale;
uniform vec3 cameraPosition;	// camera position vector
uniform float time;

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

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453) * 103 + co.x + co.y;
}

void main()
{
    // Get position of the point
    vec4 pointPosition = gl_in[0].gl_Position;

    // GET THE NORMAL VECTOR OF THE POINT ON THE TERRAIN TO ALIGN THE
    // GRASS CARD CORRECTLY
    vec2 normalTexCoord = vec2(pointPosition[0] / resX, pointPosition[2] / resZ);
    // Get normal from map for distorted texture coordinates to match the texture distortions
	vec4 normalMapColour = texture(normalMap, normalTexCoord);
	// Texture is only between 0-1, so adjust to get negative values in X-Z
	vec3 normal = vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b, normalMapColour.g * 2.0 - 1.0);
	normal = normalize(normal);

    // Find the angle and axis to rotate around
    float angle = acos(dot(normal, vec3(0.0, 1.0, 0.0)));

    vec4 worldPosition = Hwm * vec4(pointPosition[0] * scale, pointPosition[1], pointPosition[2] * scale, 1.0);
    bool farAway = length(worldPosition.xyz - cameraPosition) > 120;

    // DETERMINE CLIPPING
    // Check if need to clip vertex based on clipping plane
    float clip = dot(worldPosition, clippingPlane);

    grassType = int(mod(int(rand(vec2(positions[0]))), 4));

    if (clip < 0 || (pointPosition[1] < grassHeight || (abs(angle) > M_PI/6)) || int(mod(int(rand(vec2(grassType, grassType)) * 100), 50)) < 45 || farAway) {

    }
    else {
        gl_ClipDistance[0] = 1;

        vec3 axis = cross(normal, vec3(0.0, 1.0, 0.0));

        mat4 upRotate = rotationMatrix(axis, -angle);


        mat4 Hcm = Hcv * Hvw * Hwm;
        mat4 rotationAround[2];
        rotationAround[0] = rotationMatrix(vec3(0.0, 1.0, 0.0), 0);
        rotationAround[1] = rotationMatrix(vec3(0.0, 1.0, 0.0), (M_PI / 2) + (grassType / 6));

        float windFactor = int(mod(int(rand(vec2(positions[0] * 20))), 15)) + 5;

        float angleTime = time / windFactor;

        while (angleTime > M_PI / 3) {
            angleTime -= M_PI / 3;
        }

        vec3 wavingVector;

        if (int(mod((time / windFactor) / (M_PI / 3), 2)) == 0) {
            wavingVector = vec3(-1.0, 0.0, 0.0);
        }
        else {
            wavingVector = vec3(1.0, 0.0, 0.0);
            angleTime -= M_PI / 3;
        }

        mat4 beginWave = rotationMatrix(vec3(1.0, 0.0, 0.0), M_PI / 6);

        mat4 waving = rotationMatrix(wavingVector, angleTime);

        // CREATE SQUARES
        for (int i = 0; i < 2 ; i++) {
            // Bottom left
            gl_Position = pointPosition + ( rotationAround[i] * upRotate * (vec4(-0.5, 0.0, 0.0, 0.0) * grassScale));
            gl_Position = Hcm * vec4(gl_Position.x * scale, gl_Position.y, gl_Position.z * scale, 1.0);
            texCoords = vec2(1.0, 1.0);
            EmitVertex();

            // Bottom right
            gl_Position = pointPosition + ( rotationAround[i] * upRotate * (vec4(0.5, 0.0, 0.0, 0.0) * grassScale));
            gl_Position = Hcm * vec4(gl_Position.x * scale, gl_Position.y, gl_Position.z * scale, 1.0);
            texCoords = vec2(0.0, 1.0);
            EmitVertex();

            // Top left
            gl_Position = pointPosition + ( rotationAround[i] * upRotate * waving * beginWave * (vec4(-0.5, 1.0, 0.0, 0.0) * grassScale));
            gl_Position = Hcm * vec4(gl_Position.x * scale, gl_Position.y, gl_Position.z * scale, 1.0);
            texCoords = vec2(1.0, 0.0);
            EmitVertex();
            
            // Top right
            gl_Position = pointPosition + ( rotationAround[i] * upRotate * waving * beginWave * (vec4(0.5, 1.0, 0.0, 0.0) * grassScale));
            gl_Position = Hcm * vec4(gl_Position.x * scale, gl_Position.y, gl_Position.z * scale, 1.0);
            texCoords = vec2(0.0, 0.0);
            EmitVertex();

            EndPrimitive();
        }
    }
}