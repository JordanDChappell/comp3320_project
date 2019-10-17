#version 150
#define M_PI 3.1415926535897932384626433832795

// in variables
in vec2 TexCoord;
in vec2 normalTexCoord;				
in float height;					// height of the fragment
in vec3 toCameraVector;				// vector from vertex to camera
in vec3 FragPos;					// Position of the Fragment
//in vec3 fromLightVector;			// vector from vertex to light

// out variables
out vec4 outColor;					// colour of the fragment

// Transformation matrices
uniform mat4 Hwm;					// model to world transform
uniform mat4 Hvw;					// world to view transform
uniform mat4 Hcv;					// view to camera transform

// Textures
uniform sampler2D texGrass;			// grass texture
uniform sampler2D texRock;			// rock texture
uniform sampler2D texSand;			// sand texture
uniform sampler2D normalMap;		// normal map texture
uniform sampler2D waterNormalMap;	// normal map for underwater lighting

uniform float grassHeight;			// height value at which the terrain becomes grass
uniform float waterHeight;			// height of the water
uniform float time;					// time of the program
//uniform vec3 lightColour;			// colour of the light

// Parameters for specular lighting
const float shineDamper = 20.0;		// adjust shine
const float reflectivity = 0.6;		// adjust reflectivity



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
    vec4 result =  (ambient + diffuse + specular);

	// Under the water, add specular light from the water above
	if (height <= waterHeight) {
		// Get the water normal map texture
		vec4 waterNormalMapColour = texture(waterNormalMap, TexCoord/20 + time/25);

		// Texture is only between 0-1, so adjust to get negative values in X-Z
		vec3 waterNormal = vec3(waterNormalMapColour.r * 2.0 - 1.0, waterNormalMapColour.b, waterNormalMapColour.g * 2.0 - 1.0);
		waterNormal = normalize(waterNormal);

		// Find the specular light value based on the water normal		
		vec3 reflectedLight = reflect(normalize(lightDir), waterNormal);
		float specular1 = max(dot(reflectedLight, normalize(toCameraVector)), 0.0);
		specular1 = pow(specular1, shineDamper);
		vec3 waterSpecularHighlights = light.ambient * specular1 * reflectivity;

		// Add the specular highlights to the colour
		result = result + vec4(waterSpecularHighlights, 1.0);
	}


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

	vec4 result =  (ambient + diffuse + specular);

	// Under the water, add specular light from the water above
	if (height <= waterHeight) {
		// Get the water normal map texture
		vec4 waterNormalMapColour = texture(waterNormalMap, TexCoord/20 + time/25);

		// Texture is only between 0-1, so adjust to get negative values in X-Z
		vec3 waterNormal = vec3(waterNormalMapColour.r * 2.0 - 1.0, waterNormalMapColour.b, waterNormalMapColour.g * 2.0 - 1.0);
		waterNormal = normalize(waterNormal);

		// Find the specular light value based on the water normal		
		vec3 reflectedLight = reflect(normalize(lightDir), waterNormal);
		float specular1 = max(dot(reflectedLight, normalize(toCameraVector)), 0.0);
		specular1 = pow(specular1, shineDamper);
		vec3 waterSpecularHighlights = light.ambient * specular1 * reflectivity;

		// Add the specular highlights to the colour
		result = result + vec4(waterSpecularHighlights, 1.0);
	}


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
	vec4 result =  (ambient + diffuse + specular);

	// Under the water, add specular light from the water above
	if (height <= waterHeight) {
		// Get the water normal map texture
		vec4 waterNormalMapColour = texture(waterNormalMap, TexCoord/20 + time/25);

		// Texture is only between 0-1, so adjust to get negative values in X-Z
		vec3 waterNormal = vec3(waterNormalMapColour.r * 2.0 - 1.0, waterNormalMapColour.b, waterNormalMapColour.g * 2.0 - 1.0);
		waterNormal = normalize(waterNormal);

		// Find the specular light value based on the water normal		
		vec3 reflectedLight = reflect(normalize(lightDir), waterNormal);
		float specular1 = max(dot(reflectedLight, normalize(toCameraVector)), 0.0);
		specular1 = pow(specular1, shineDamper);
		vec3 waterSpecularHighlights = light.ambient * specular1 * reflectivity;

		// Add the specular highlights to the colour
		result = result + vec4(waterSpecularHighlights, 1.0);
	}


    return result;
}

void main()
{
	//-----------
	// GET NORMAL
	//-----------
	// Get normal from map for terrain normals
	vec4 normalMapColour = texture(normalMap, normalTexCoord);
	// Texture is only between 0-1, so adjust to get negative values in X-Z
	vec3 normal = vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b, normalMapColour.g * 2.0 - 1.0);
	normal = normalize(normal);

	float slope = acos(dot(normal, vec3(0, 1, 0)));

	//------------
	// GET TEXTURE
	//------------

	vec4 colTexture;
	vec4 grassTexture = texture(texGrass, TexCoord);
	vec4 rockTexture = texture(texRock, TexCoord);
	vec4 sandTexture = texture(texSand, TexCoord);

	// Texture will just be rock
	if (height < (grassHeight - 1)) {
		colTexture = sandTexture;
	}

	// Fade in the grass
	else if (height <= grassHeight && height >= (grassHeight - 1)) {
		colTexture = mix(sandTexture, grassTexture, height - (grassHeight - 1));
	}

	// Only grass
	else {
		colTexture = grassTexture;	
	} 

	// Add rocks to areas that slope a lot
	if (slope > M_PI/6) {
		colTexture = rockTexture;
	}

	// properties
    //vec3 norm = vec3(normalMapColour.x, normalMapColour.y, normalMapColour.z);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // lighting set up in 3 phases directional, point lights and an optional flashlight
    // phase 1: directional lighting
    vec4 result = CalcDirLight(dirLight, normal, viewDir, colTexture);
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir, colTexture);    
    // phase 3: spot light
    result += CalcSpotLight(spotLight, normal, FragPos, viewDir, colTexture);    
    
    outColor = result;
	
	
}
