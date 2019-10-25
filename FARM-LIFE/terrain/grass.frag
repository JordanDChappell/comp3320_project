#version 330 core

in vec2 texCoords;
flat in int grassType;
//in vec3 FragPos;
in vec3 norm;
in vec3 toCameraVector;

// out variables
out vec4 outColour;					// colour of the fragment

uniform sampler2D grassTex1;
uniform sampler2D grassTex2;
uniform sampler2D grassTex3;
uniform sampler2D grassTex4;


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
vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 colTexture);
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 colTexture);
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 colTexture);

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



    return result;
}

void main()
{
	//-----------
	// GET NORMAL
	//-----------
	vec3 normal = norm;

	vec4 texColour;
     if (grassType == 0) {
         texColour = texture(grassTex1, texCoords);
     }
     else if (grassType == 1) {
         texColour = texture(grassTex2, texCoords);
     }
     else if (grassType == 2) {
         texColour = texture(grassTex3, texCoords);
     }
     else {
         texColour = texture(grassTex4, texCoords);
     }
    //vec4 texColour = texture(grassTex1, texCoords);
    
    if(texColour[3] < 0.5) {
        discard;
    }


    //outColour = texColour;

	vec4 pointPosition = gl_FragCoord;
	vec3 FragPos = pointPosition.xyz; 

	// properties

    vec3 viewDir = normalize(viewPos - FragPos);
    
    // lighting set up in 3 phases directional, point lights and an optional flashlight
    // phase 1: directional lighting
    vec4 result = CalcDirLight(dirLight, normal, viewDir, texColour);
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir, texColour);    
    // phase 3: spot light
    result += CalcSpotLight(spotLight, normal, FragPos, viewDir, texColour);    
    //result = result * vec4(0.1f,0.1f,0.1f,1.0f)
    outColour = result;
		
}

/*
void main()
{
    vec4 texColour;
     if (grassType == 0) {
         texColour = texture(grassTex1, texCoords);
     }
     else if (grassType == 1) {
         texColour = texture(grassTex2, texCoords);
     }
     else if (grassType == 2) {
         texColour = texture(grassTex3, texCoords);
     }
     else {
         texColour = texture(grassTex4, texCoords);
     }
    //vec4 texColour = texture(grassTex1, texCoords);
    
    if(texColour[3] < 0.5) {
        discard;
    }


    outColour = texColour;
}  
*/
