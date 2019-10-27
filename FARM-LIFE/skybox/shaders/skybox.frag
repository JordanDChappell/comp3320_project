#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float currTime; //time in seconds minus the floor of time, divided by 240 to give time as a val between 0-1 representing 240 seconds, or four minutes for day and night being two minutes each.

void main()
{    
	float time = currTime * 2 * 3.1415926535f;
	float lightLevel = ((1 - (sin(time) / 0.05f))* 0.9f);
	if(currTime>0.50){
		FragColor = texture(skybox, TexCoords);
	}else if( time < 0.05 && time>0){
		
		//float time = currTime * 2 * 3.14159f;
		//float lightLevel = ((1 - (sin(time) / 0.05f))* 0.9f)+0.1f);
		vec4 sky = texture(skybox, TexCoords);
		sky = sky*vec4(lightLevel, lightLevel, lightLevel, 1.0);
		FragColor = sky;
	}else {
		vec4 sky = texture(skybox, TexCoords);
		sky = sky*vec4(0.1f, 0.1f, 0.1f, 1.0);
		FragColor = sky;
	}
   
}