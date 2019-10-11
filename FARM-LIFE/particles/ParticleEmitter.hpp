#ifndef ASSIGNMENT_PARTICLE_EMITTER
#define ASSIGNMENT_PARTICLE_EMITTER

#include "Particle.hpp"

namespace particle {

    class ParticleEmitter {
    public:
    virtual ~ParticleEmitter() {}
    virtual void EmitParticle(Particle& particle) = 0;
    virtual void DebugRender() {}

    };

}

#endif  // ASSIGNMENT_PARTICLE_EMITTER