#ifndef ASSIGNMENT_PARTICLES_HPP
#define ASSIGNMENT_PARTICLES_HPP

#include "glm/glm.hpp"
#include <vector>

namespace particle
{
	/**
	 * Particle properties.
	 * 
	 * These properties dictate each particles shape, size, colour, location, 
	 * travel speed and direction as well as perpendicular orientation to 
	 * camera view.
	 */
	class Particle
	{
    public:
		typedef std::vector<Particle> ParticleBuffer;

		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec4 colour;
		float fRotate;
		float fSize;
		float fAge;
		float fLifeTime;

		Particle() {
			position = glm::vec3(0);
			velocity = glm::vec3(0);
			colour = glm::vec4(0);
			fRotate = 0;
			fSize = 0;
			fAge = 0;
			fLifeTime = 0;
		}
	};
}

#endif