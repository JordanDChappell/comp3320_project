#pragma once

#include <vector>

namespace particle
{
	struct Particle
	{
		Particle() :
			position(0),
			velocity(0),
			colour(0),
			fRotate(0),
			fAge(0),
			fLifeTime(0)
		{}

		glm::vec3 position; // Center point of particle
		glm::vec3 velocity; // Current particle velocity
		glm::vec4 colour;    // Particle color
		float     fRotate;  // Rotate the particle the center
		float     fSize;    // Size of the particle
		float     fAge;
		float     fLifeTime;
	};

	typedef std::vector<Particle> ParticleBuffer;
}
