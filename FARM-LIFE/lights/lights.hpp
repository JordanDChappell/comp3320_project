

#ifndef A1_LIGHTS_HPP
#define A1_LIGHTS_HPP
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace lights
{

	class light
	{
	public:
		light(GLuint shader)
		{
			this->shader = shader;
		};

		GLuint setup(glm::vec3 CamPos, glm::vec3 Forward)
		{

			/*
			Here we set all the uniforms for the 6 lights we have. We have to set them manually
			*/

			glUniform3f(glGetUniformLocation(shader, "viewPos"), CamPos.x, CamPos.y, CamPos.z);


			// directional light

			glm::vec3 lightDirection(0.0f, 0.0f, 0.1f);
			float time = glfwGetTime() / 240;
			time = (time - floor(time)) * 2 * M_PI;
			lightDirection.x = cos(time);
			lightDirection.y = sin(time);
			float diffuse = (1 - (lightDirection.y / 0.05f)) * 0.6f;
			float specular = (1 - (lightDirection.y / 0.05f)) * 0.2f;
			float ambient = (1 - (lightDirection.y / 0.05f)) * 0.05f;

			glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), lightDirection.x, lightDirection.y, lightDirection.z);

			if (lightDirection.y > 0.05) {
				glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.005f, 0.005f, 0.005f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.0f, 0.0f, 0.0f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 0.01f, 0.01f, 0.01f);
			}
			else if (lightDirection.y < 0.05 && lightDirection.y>0) {
				//std::cout << " " << diffuse << " ";
				glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), ambient, ambient, ambient);
				glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), diffuse, diffuse, diffuse);
				glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), specular, specular, specular);
			}
			else if (lightDirection.y < 0) {
				glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 0.2f, 0.2f, 0.2f);
			}


			// point light 1
			glUniform3f(glGetUniformLocation(shader, "pointLights[0].position"), 1.0f, -9.83f, 2.0f);
			glUniform3f(glGetUniformLocation(shader, "pointLights[0].ambient"), 0.1f, 0.1f, 0.1f);
			glUniform3f(glGetUniformLocation(shader, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
			glUniform3f(glGetUniformLocation(shader, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(shader, "pointLights[0].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shader, "pointLights[0].linear"), 0.09);
			glUniform1f(glGetUniformLocation(shader, "pointLights[0].quadratic"), 0.032);
			// point light 2
			glUniform3f(glGetUniformLocation(shader, "pointLights[1].position"), 50.0f, 3.57f, 80.0f);
			glUniform3f(glGetUniformLocation(shader, "pointLights[1].ambient"), 0.1f, 0.1f, 0.1f);
			glUniform3f(glGetUniformLocation(shader, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
			glUniform3f(glGetUniformLocation(shader, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(shader, "pointLights[1].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shader, "pointLights[1].linear"), 0.09);
			glUniform1f(glGetUniformLocation(shader, "pointLights[1].quadratic"), 0.032);

			// point light 3
			glUniform3f(glGetUniformLocation(shader, "pointLights[2].position"), 100.0f, 6.28f, 1.0f);
			glUniform3f(glGetUniformLocation(shader, "pointLights[2].ambient"), 0.1f, 0.1f, 0.1f);
			glUniform3f(glGetUniformLocation(shader, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
			glUniform3f(glGetUniformLocation(shader, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(shader, "pointLights[2].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shader, "pointLights[2].linear"), 0.09);
			glUniform1f(glGetUniformLocation(shader, "pointLights[2].quadratic"), 0.032);
			// point light 4
			glUniform3f(glGetUniformLocation(shader, "pointLights[3].position"), 10.0f, 0.63f, 50.0f);
			glUniform3f(glGetUniformLocation(shader, "pointLights[3].ambient"), 0.1f, 0.1f, 0.1f);
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

} // namespace lights

#endif // LIGHTS_LIGHTS_HPP
