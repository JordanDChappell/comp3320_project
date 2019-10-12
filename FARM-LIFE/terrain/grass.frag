#version 330 core

in vec2 texCoords;
flat in int grassType;

// out variables
out vec4 outColour;					// colour of the fragment

uniform sampler2D grassTex1;
uniform sampler2D grassTex2;
uniform sampler2D grassTex3;
uniform sampler2D grassTex4;

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