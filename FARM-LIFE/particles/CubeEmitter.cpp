#include "ParticleSystemPCH.h"
#include "Random.h"
#include "CubeEmitter.h"

namespace particle
{
	CubeEmitter::CubeEmitter():
		MinWidth(-1),
		MaxWidth(1),
		MinHeight(-1),
		MaxHeight(1),
		MinDepth(-1),
		MaxDepth(1),
		MinSpeed(10),
		MaxSpeed(20),
		MinLifetime(3),
		MaxLifetime(5),
		Origin(0)
	{}

	void CubeEmitter::EmitParticle(Particle& particle)
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

	void CubeEmitter::DebugRender()
	{
		glTranslatef(Origin.x, Origin.y, Origin.z);

		glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
    
		glutWireCube(MaxWidth * 2);

		glTranslatef(-Origin.x, -Origin.y, -Origin.z);
	}
}
