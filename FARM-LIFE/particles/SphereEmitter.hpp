#ifndef ASSIGNMENT_SPHERE_EMITTER_HPP
#define ASSIGNMENT_SPHERE_EMITTER_HPP

#include "ParticleEmitter.hpp"
#include <GL/glew.h>
#include "glm/gtc/type_ptr.hpp"

namespace particle
{
	/**
	 * Sphere Emitter for particle effects
	 *
	 * An invisible sphere will be created with a given radius and placed 
	 * in the world. This sphere will generate specified particles within 
	 * the circles radius and act in some way over its life according to 
	 * the particle effects.
	 */
    class SphereEmitter : public ParticleEmitter
	{
    public:
   		float MinimumRadius, MaximumRadius;
		float MinInclination, MaxInclination;
		float MinAzimuth, MaxAzimuth;
		float MinSpeed, MaxSpeed;
		float MinLifetime, MaxLifetime;
		glm::vec3 Origin;
    
        SphereEmitter()
		{
            MinimumRadius = 0;
            MaximumRadius = 1;
            MinInclination = 0;
            MaxInclination = 180;
            MinAzimuth = 0;
            MaxAzimuth = 360;
            MinSpeed = 10;
            MaxSpeed = 20;
            MinLifetime = 3;
            MaxLifetime = 5;
            Origin = glm::vec3(0);
        }

        ~SphereEmitter() {}

        void EmitParticle(Particle& particle)
		{
            float inclination = glm::radians(RandRange(MinInclination, MaxInclination));
            float azimuth = glm::radians(RandRange(MinAzimuth, MaxAzimuth));

            float radius = RandRange(MinimumRadius, MaximumRadius);
            float speed = RandRange(MinSpeed, MaxSpeed);
            float lifetime = RandRange(MinLifetime, MaxLifetime);

            float sInclination = sinf(inclination);

            float X = sInclination * cosf(azimuth);
            float Y = sInclination * sinf(azimuth);
            float Z = cosf(inclination);

            glm::vec3 vector(X, Y, Z);

            particle.position = (vector * radius) + Origin;
            particle.velocity = vector * speed;

            particle.fLifeTime = lifetime;
            particle.fAge = 0;
	    }

        void DebugRender()
		{
            glTranslatef(Origin.x, Origin.y, Origin.z);

			// Command below left for alternate debugging instance
            // RenderSphere(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), MinimumRadius);
            RenderSphere(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), MaximumRadius);

            glTranslatef(-Origin.x, -Origin.y, -Origin.z);
	    }
    private:
		// Only used for debugging purposes
        void RenderSphere(glm::vec4 color, float fRadius)
		{
            float X, Y, Z, inc, azi;

            glColor4fv(glm::value_ptr(color));

            glPointSize(2.0f);
            glBegin(GL_POINTS);

            for (float azimuth = MinAzimuth; azimuth < MaxAzimuth; azimuth += 5.0f)
            {
                for (float inclination = MinInclination; inclination < MaxInclination; inclination += 5.0f)
                {
                    inc = glm::radians(inclination);
                    azi = glm::radians(azimuth);

                    X = fRadius * sinf(inc) * cosf(azi);
                    Y = fRadius * sinf(inc) * sinf(azi);
                    Z = fRadius * cosf(inc);

                    glVertex3f(X, Y, Z);
                }

                inc = glm::radians(MaxInclination);
                azi = glm::radians(azimuth);

                X = fRadius * sinf(inc) * cosf(azi);
                Y = fRadius * sinf(inc) * sinf(azi);
                Z = fRadius * cosf(inc);

                glVertex3f(X, Y, Z);
            }

            inc = glm::radians(MaxInclination);
            azi = glm::radians(MaxAzimuth);

            X = MaximumRadius * sinf(inc) * cosf(azi);
            Y = MaximumRadius * sinf(inc) * sinf(azi);
            Z = MaximumRadius * cosf(inc);

            glVertex3f(X, Y, Z);

            glEnd();
	    }
    };
}

#endif