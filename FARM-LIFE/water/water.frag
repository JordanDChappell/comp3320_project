#version 150

in vec4 clipSpace;
in vec2 texCoords;
in vec3 toCameraVector;
in vec3 fromLightVector; 

out vec4 outColour;

// Reflection in the water
uniform sampler2D reflectionTexture;
// Refraction in the water
uniform sampler2D refractionTexture;
// Dudv map for ripples
uniform sampler2D dudvMap;
// Normal map for lighting
uniform sampler2D normalMap;
// Time for changing ripples
uniform float time;
// Colour to add blue tint
uniform vec3 colour;

uniform vec3 lightColour;

const float shineDamper = 20.0;
const float reflectivity = 0.6;

void main()
{
	vec2 ndc = ((clipSpace.xy / clipSpace.w) / 2.0) + 0.5;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
	vec2 refractTexCoords = vec2(ndc.x, ndc.y);

	vec2 distortedTexCoords = texture(dudvMap, vec2(texCoords.x + time / 20, texCoords.y)).rg * 0.1;
	distortedTexCoords = texCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + time / 20);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * 0.05;

	reflectTexCoords += totalDistortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);

	refractTexCoords += totalDistortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	vec4 reflectColour = texture(reflectionTexture, reflectTexCoords);
	vec4 refractColour = texture(refractionTexture, refractTexCoords);
	
	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));
	refractiveFactor = pow(refractiveFactor, 0.7);

	vec4 normalMapColour = texture(normalMap, distortedTexCoords);
	vec3 normal = vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b, normalMapColour.g * 2.0 - 1.0);
	normal = normalize(normal);

	vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = lightColour * specular * reflectivity;

	outColour = mix(reflectColour, refractColour, refractiveFactor);
	outColour = mix(outColour, vec4(colour, 1.0), 0.05) + vec4(specularHighlights, 1.0);
}