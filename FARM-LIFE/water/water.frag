#version 150

// in variables
in vec4 clipSpace;						// fragment position in clip space
in vec2 texCoords;						// texture coordinates (x,y) of this fragment
in vec3 toCameraVector;					// vector from fragment to camera
in vec3 FragPos;						// Position of the Fragment
//in vec3 fromLightVector; 				// vector from fragment to light vector

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
//uniform vec3 lightColour;				// colour of the light
uniform float near;						// near plane of program
uniform float far;						// far plane of program
uniform bool isCameraAbove;				// check if we need to do reflectivity or not

// Parameters for specular lighting
const float shineDamper = 50.0;			// adjust shine
const float reflectivity = 0.6;			// adjust reflectivity

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 4

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

// calculates the color when using a directional light.
vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 colTexture)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.0);
    // combine results
    vec4 ambient = vec4(light.ambient, 1.0) * colTexture;
    vec4 diffuse = vec4(light.diffuse, 1.0) * diff * colTexture;
    vec4 specular = vec4(light.specular, 1.0) * spec * colTexture;

	vec4 result = ambient + diffuse + specular;

	// Find the specular light value based on the water normal		
	vec3 reflectedLight = reflect(normalize(lightDir), normal);
	float specular1 = max(dot(reflectedLight, normalize(toCameraVector)), 0.0);
	specular1 = pow(specular1, shineDamper);
	vec3 waterSpecularHighlights = light.ambient * specular1 * reflectivity;

	// Add the specular highlights to the colour
	result += vec4(waterSpecularHighlights, 1.0);

    return result;
}

// calculates the color when using a point light.
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 colTexture)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec4 ambient = vec4(light.ambient, 1.0) * colTexture;
    vec4 diffuse = vec4(light.diffuse, 1.0) * diff * colTexture;
    vec4 specular = vec4(light.specular, 1.0) * spec * colTexture;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

	vec4 result = ambient + diffuse + specular;

	// Find the specular light value based on the water normal		
	vec3 reflectedLight = reflect(normalize(lightDir), normal);
	float specular1 = max(dot(reflectedLight, normalize(toCameraVector)), 0.0);
	specular1 = pow(specular1, shineDamper);
	vec3 waterSpecularHighlights = light.ambient * specular1 * reflectivity;

	// Add the specular highlights to the colour
	result += vec4(waterSpecularHighlights, 1.0);

    return result;
}

// calculates the color when using a spot light.
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 colTexture)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec4 ambient = vec4(light.ambient, 1.0) * colTexture;
    vec4 diffuse = vec4(light.diffuse, 1.0) * diff * colTexture;
    vec4 specular = vec4(light.specular, 1.0) * spec * colTexture;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

	vec4 result = ambient + diffuse + specular;

	// Find the specular light value based on the water normal		
	vec3 reflectedLight = reflect(normalize(lightDir), normal);
	float specular1 = max(dot(reflectedLight, normalize(toCameraVector)), 0.0);
	specular1 = pow(specular1, shineDamper);
	vec3 waterSpecularHighlights = light.ambient * specular1 * reflectivity;

	// Add the specular highlights to the colour
	result += vec4(waterSpecularHighlights, 1.0);

    return result;
}


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
	vec3 normal = vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b * 2, normalMapColour.g * 2.0 - 1.0);
	normal = normalize(normal);

	//---------------------
	// APPLY FRESNEL EFFECT
	//---------------------
	// Get the view vector
	vec3 viewVector = normalize(toCameraVector);
	// Get angle with the direct up vector
	float refractiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));
	refractiveFactor = pow(refractiveFactor, 0.7);

	//-----------------------
	// SET COLOUR OF FRAGMENT
	//-----------------------
	// Reflective vs refractive with Fresnel effect
	outColour = mix(reflectColour, refractColour, refractiveFactor);
	if (!isCameraAbove) {
		outColour = refractColour;
	}
	// Add colour tint
	outColour = mix(outColour, vec4(colour, 1.0), 0.05);

	//---------------
	// APPLY LIGHTING
	//---------------
	// properties
    vec3 viewDir = normalize(viewPos - FragPos);
	// lighting set up in 3 phases directional, point lights and an optional flashlight
    // phase 1: directional lighting
    vec4 result = CalcDirLight(dirLight, normal, viewDir, outColour);
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir, outColour);    
    // phase 3: spot light
    result += CalcSpotLight(spotLight, normal, FragPos, viewDir, outColour);    

	outColour = result;

	// Adjust alpha value based on water depth
	outColour.a = clamp(waterDepth/2, 0.0, 1.0);
}