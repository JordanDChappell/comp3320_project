#version 150

// in variables
in vec4 clipSpace;						// fragment position in clip space
in vec2 texCoords;						// texture coordinates (x,y) of this fragment
in vec3 toCameraVector;					// vector from fragment to camera
in vec3 fromLightVector; 				// vector from fragment to light vector

// out variables
out vec4 outColour;						// colour of the fragment

// Textures and maps
uniform sampler2D reflectionTexture;	// reflection texture
uniform sampler2D refractionTexture;	// refraction texture
uniform sampler2D dudvMap;				// du/dv map for texture distortion
uniform sampler2D normalMap;			// normal map for lighting
uniform sampler2D depthMap;				// refraction depth map for alpha blending shallow water

// Other uniforms
uniform float time;						// time for changing ripples
uniform vec3 colour;					// colour to add blue tint
uniform vec3 lightColour;				// colour of the light
uniform float near;						// near plane of program
uniform float far;						// far plane of program
uniform bool isCameraAbove;				// check if we need to do reflectivity or not

// Parameters for specular lighting
const float shineDamper = 20.0;			// adjust shine
const float reflectivity = 0.6;			// adjust reflectivity

void main()
{
	// ---------------------------------------------------------------
	// GET DISTORTED REFLECTION AND REFRACTION COLOUR OF THIS FRAGMENT
	// ---------------------------------------------------------------

	// Get the fragment in normalised device coordinates
	vec2 ndc = ((clipSpace.xy / clipSpace.w) / 2.0) + 0.5;

	// Set the texture coordinates based on the normalised device coordinates
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);	
	vec2 refractTexCoords = vec2(ndc.x, ndc.y);

	// Distort the texture coordinates to create ripples using the du/dv map
	vec2 distortedTexCoords = texture(dudvMap, vec2(texCoords.x + time / 20, texCoords.y)).rg * 0.1;
	distortedTexCoords = texCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + time / 20);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * 0.05;

	// Displace texture coordinates by the distortion and clamp to prevent abnormalities
	reflectTexCoords += totalDistortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);		// reflection is flipped in y-axis

	refractTexCoords += totalDistortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	// Get the texture colour from the texture and the adjusted coordinates
	vec4 reflectColour = texture(reflectionTexture, reflectTexCoords);
	vec4 refractColour = texture(refractionTexture, refractTexCoords);

	//----------------
	// CALCULATE DEPTH
	//----------------
	// Get the depth of the fragment (black and white, so only sample red)
	float depth = texture(depthMap, refractTexCoords).r;

	// Calculate the distance to the river floor from the camera
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	// Change depth to the fragment to get the fragment/water distance to camera
	depth = gl_FragCoord.z;
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	// Get distance from fragment/water to the bottom of the river
	float waterDepth = floorDistance - waterDistance;

	//---------------------------
	// GET NORMAL OF THE FRAGMENT
	//---------------------------
	// Get normal from map for distorted texture coordinates to match the texture distortions
	vec4 normalMapColour = texture(normalMap, distortedTexCoords);
	// Texture is only between 0-1, so adjust to get negative values in X-Z
	vec3 normal = vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b, normalMapColour.g * 2.0 - 1.0);
	normal = normalize(normal);

	//---------------------
	// APPLY FRESNEL EFFECT
	//---------------------
	// Get the view vector
	vec3 viewVector = normalize(toCameraVector);
	// Get angle with the direct up vector
	float refractiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));
	refractiveFactor = pow(refractiveFactor, 0.7);

	//---------------------
	// APPLY SPECULAR LIGHT
	//---------------------
	vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = lightColour * specular * reflectivity * clamp(waterDepth/2, 0.0, 1.0);

	//-----------------------
	// SET COLOUR OF FRAGMENT
	//-----------------------
	// Reflective vs refractive with Fresnel effect
	outColour = mix(reflectColour, refractColour, refractiveFactor);
	if (!isCameraAbove) {
		outColour = refractColour;
	}
	// Add colour tint and add specular highlights
	outColour = mix(outColour, vec4(colour, 1.0), 0.05) + vec4(specularHighlights, 1.0);
	// Adjust alpha value based on water depth
	outColour.a = clamp(waterDepth/2, 0.0, 1.0);
}