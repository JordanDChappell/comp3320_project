#pragma once;

#include "Particle.h"

namespace particle
{
	class ParticleEmitter
	{
	public:
		virtual ~ParticleEmitter() {}
		virtual void EmitParticle(Particle& particle) = 0;

		virtual void DebugRender() {}
	};
}
