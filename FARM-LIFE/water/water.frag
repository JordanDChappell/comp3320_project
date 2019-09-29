#version 150

in vec4 Colour;
in vec4 clipSpace;
in vec2 texCoords;

out vec4 outColour;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform float time;

void main()
{
	vec2 ndc = ((clipSpace.xy / clipSpace.w) / 2.0) + 0.5;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
	vec2 refractTexCoords = vec2(ndc.x, ndc.y);

	vec2 distortion1 = (texture(dudvMap, vec2(texCoords.x + time / 15, texCoords.y)).rg * 2.0 - 1.0) * 0.05;
	vec2 distortion2 = (texture(dudvMap, vec2(-texCoords.x + time / 30, texCoords.y + time / 30)).rg * 2.0 - 1.0) * 0.05;

	vec2 totaDistortion = distortion1 + distortion2;

	reflectTexCoords += totaDistortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);

	refractTexCoords += totaDistortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	vec4 reflectColour = texture(reflectionTexture, reflectTexCoords);
	vec4 refractColour = texture(refractionTexture, refractTexCoords);
	
	outColour = mix(refractColour, reflectColour, 0.5);
}