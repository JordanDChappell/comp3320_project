#ifndef ASSIGNMENT_PARTICLE_EMITTER
#define ASSIGNMENT_PARTICLE_EMITTER

#include "Particle.hpp"
#include "Random.hpp"

namespace particle
{
	/**
	 * Base class for particle emitter.
	 *
	 * Emitter is responsible for describing the generation of particles.
	 * This includes the lifetime bounds for particles, the speed at which 
	 * they move and the location they appear.
	 */
    class ParticleEmitter
	{
    public:
		virtual ~ParticleEmitter() {}
		virtual void EmitParticle(Particle& particle) = 0;
		virtual void DebugRender() {}
    };
}

#endif