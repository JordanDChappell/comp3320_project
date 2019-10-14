

#ifndef A1_LIGHTS_HPP
#define A1_LIGHTS_HPP
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


namespace lights {

	class light
	{
	public:
		
		light(GLuint shader) {
			this->shader = shader;
		};
		
		
		GLuint setup(glm::vec3 CamPos, glm::vec3 Forward) {

		/*
		Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/

		
		// directional light
		glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
		// point light 1
		glUniform3f(glGetUniformLocation(shader, "pointLights[0].position"), 0.7f, 0.2f, 2.0f);
		glUniform3f(glGetUniformLocation(shader, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader, "pointLights[0].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader, "pointLights[0].quadratic"), 0.032);
		// point light 2
		glUniform3f(glGetUniformLocation(shader, "pointLights[1].position"), 2.3f, -3.3f, -4.0f);
		glUniform3f(glGetUniformLocation(shader, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader, "pointLights[1].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader, "pointLights[1].quadratic"), 0.032);
		
		// point light 3
		glUniform3f(glGetUniformLocation(shader, "pointLights[2].position"), -4.0f, 2.0f, -12.0f);
		glUniform3f(glGetUniformLocation(shader, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader, "pointLights[2].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader, "pointLights[2].quadratic"), 0.032);
		// point light 4
		glUniform3f(glGetUniformLocation(shader, "pointLights[3].position"), 0.0f, 0.0f, -3.0f);
		glUniform3f(glGetUniformLocation(shader, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader, "pointLights[3].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader, "pointLights[3].quadratic"), 0.032);
		// spotLight If we want one?
		glUniform3f(glGetUniformLocation(shader, "spotLight.position"), CamPos.x, CamPos.y, CamPos.z);
		glUniform3f(glGetUniformLocation(shader, "spotLight.direction"), Forward.x, Forward.y, Forward.z);
		glUniform3f(glGetUniformLocation(shader, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader, "spotLight.linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader, "spotLight.quadratic"), 0.032);
		glUniform1f(glGetUniformLocation(shader, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(shader, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));
		return shader;
		
		};

	private:
		GLuint shader;

	};


}


#endif  // LIGHTS_LIGHTS_HPP