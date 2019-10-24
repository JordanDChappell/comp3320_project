

#ifndef A1_LIGHTS_HPP
#define A1_LIGHTS_HPP

#define M_PI 3.1415926535897932384626433832795

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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
		Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/

		glUniform3f(glGetUniformLocation(shader, "viewPos"), CamPos.x, CamPos.y, CamPos.z);
		// directional light
		glm::vec3 lightDirection(0.0f, 0.0f, 0.0f);
		float daytime = glfwGetTime() / 240;

		//EXPERIMENTAL
		float time = glfwGetTime() / 240;
		time = (time - floor(time)) * 2 * M_PI;
		lightDirection.x = 500 * cos(time);
		lightDirection.y = 500 * sin(time);

		glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), lightDirection.x, lightDirection.y, lightDirection.z);
		glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 0.1f, 0.1f, 0.1f);
		// END EXPERIMENTAL

		//daytime = daytime - floor(daytime);
		//std::cout << " " << daytime << " ";
		/*if (daytime < 0.49)
		{ //daytime
			if (daytime < 0.2499 && daytime < 0.05)
			{ //sun rising starting
				glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), (-1.0f + (-daytime * 4)), (-daytime * 4), -0.1f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 0.2f, 0.2f, 0.2f);
			}
			else if (daytime < 0.2499 && daytime < 0.1)
			{ //sun rising slowly
				glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), (-1.0f + (daytime * 4)), (-daytime * 4), -0.1f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 0.2f, 0.2f, 0.2f);
			}
			else if (daytime < 0.2499)
			{ //sun rising fully
				glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), (-1.0f + (daytime * 4)), (-daytime * 4), -0.1f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 0.2f, 0.2f, 0.2f);
			}
			else if (daytime > 0.2498 && daytime < 0.4)
			{ //sun setting
				glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), (-1.0f + (daytime * 4)), -(2.0f - (daytime * 4)), -0.1f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 0.2f, 0.2f, 0.2f);
			}
			else if (daytime > 0.2498 && daytime < 0.45)
			{ //sun setting lighting lowering
				glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), (-1.0f + (daytime * 4)), -(2.0f - (daytime * 4)), -0.1f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 0.2f, 0.2f, 0.2f);
			}
			else
			{ //sun setting lighting lowering to final amount
				glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), (-1.0f + (daytime * 4)), -(2.0f - (daytime * 4)), -0.1f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
				glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 0.1f, 0.1f, 0.1f);
			}
		}
		else
		{ //nighttime
			glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), -0.2f, -1.0f, -0.1f);
			glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
			glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
			glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 0.1f, 0.1f, 0.1f);
		}*/

		/*
		glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), c);
		*/
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

} // namespace lights

#endif // LIGHTS_LIGHTS_HPP
