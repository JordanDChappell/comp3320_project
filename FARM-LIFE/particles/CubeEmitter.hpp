#ifndef ASSIGNMENT_CUBE_EMITTER_HPP
#define ASSIGNMENT_CUBE_EMITTER_HPP

#include "ParticleEmitter.hpp"

namespace particle
{
	/**
	 * Cube Emitter for particle effects
	 *
	 * An invisible cube will be created and placed in the world. This
	 * cube will generate specified particles within the cubes bounds 
	 * and act in some way over its life according to the particle effects.
	 */
    class CubeEmitter : public ParticleEmitter
	{
	public:
		float MinWidth, MaxWidth;
		float MinHeight, MaxHeight;
		float MinDepth, MaxDepth;
		float MinSpeed, MaxSpeed;
		float MinLifetime, MaxLifetime;
		glm::vec3 Origin;

		CubeEmitter()
		{
			MinWidth = -1;
			MaxWidth = 1;
			MinHeight = -1;
			MaxHeight = 1;
			MinDepth = -1;
			MaxDepth = 1;
			MinSpeed = 10;
			MaxSpeed = 20;
			MinLifetime = 3;
			MaxLifetime = 5;
			Origin = glm::vec3(0);
		}

		~CubeEmitter() {}

		void EmitParticle(Particle& particle)
		{
			float X = RandRange(MinWidth, MaxWidth);
			float Y = RandRange(MinHeight, MaxHeight);
			float Z = RandRange(MinDepth, MaxDepth);

			float lifetime = RandRange(MinLifetime, MaxLifetime);
			float speed = RandRange(MinSpeed, MaxSpeed);

			glm::vec3 vector(X, Y, Z);

			particle.position = vector + Origin;
			particle.velocity = glm::normalize(vector) * speed;

			particle.fLifeTime = lifetime;
			particle.fAge = 0;
		}

		void DebugRender()
		{
			glTranslatef(Origin.x, Origin.y, Origin.z);
			glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
			//glutWireCube(MaxWidth * 2);
			glTranslatef(-Origin.x, -Origin.y, -Origin.z);
		}
    };
}

#endif