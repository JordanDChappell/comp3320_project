#version 150

in vec4 clipSpace;
in vec2 texCoords;
in vec3 toCameraVector;

out vec4 outColour;

// Reflection in the water
uniform sampler2D reflectionTexture;
// Refraction in the water
uniform sampler2D refractionTexture;
// Dudv map for ripples
uniform sampler2D dudvMap;
// Time for changing ripples
uniform float time;
// Colour to add blue tint
uniform vec3 colour;

void main()
{
	vec2 ndc = ((clipSpace.xy / clipSpace.w) / 2.0) + 0.5;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
	vec2 refractTexCoords = vec2(ndc.x, ndc.y);

	vec2 distortion1 = (texture(dudvMap, vec2(texCoords.x + time / 15, texCoords.y)).rg * 2.0 - 1.0) * 0.02;
	vec2 distortion2 = (texture(dudvMap, vec2(-texCoords.x + time / 30, texCoords.y + time / 30)).rg * 2.0 - 1.0) * 0.02;

	vec2 totalDistortion = distortion1 + distortion2;

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

	outColour = mix(reflectColour, refractColour, refractiveFactor);
	outColour = mix(outColour, vec4(colour, 1.0), 0.05);
}