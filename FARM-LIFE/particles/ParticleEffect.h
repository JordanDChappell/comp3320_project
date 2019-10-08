#pragma once

#include "Particle.h"
#include "Interpolator.h"

namespace particle
{
	class ParticleEmitter;

	class ParticleEffect
	{
	public:
		// A vertex for the particle
		struct Vertex
		{
			Vertex() :
				pos(0),
				diffuse(0),
				tex0(0)
			{}

			glm::vec3 pos;		// Vertex position
			glm::vec4 diffuse;	// Diffuse colour
			glm::vec2 tex0;		// Texture coordinate
		};
		typedef std::vector<Vertex> VertexBuffer;
		typedef Interpolator<glm::vec4> ColourInterpolator;

		ParticleEffect(unsigned int numParticles = 0);
		virtual ~ParticleEffect();

		void SetCamera(utility::camera::Camera* pCamera);
		void SetParticleEmitter(ParticleEmitter* pEmitter);
		void SetColourInterplator(const ColourInterpolator& colors);

		// Test method to randomize the particles in an interesting way
		void RandomizeParticles();
		void EmitParticles();

		virtual void Update(float fDeltaTime);
		virtual void Render();

		bool LoadTexture(const std::string& fileName);

		// Resize the particle buffer with numParticles
		void Resize(unsigned int numParticles);
	protected:
		void RandomizeParticle(Particle& particle);
		void EmitParticle(Particle& particle);
	public:
		// Build the vertex buffer from the particle buffer
		void BuildVertexBuffer();
	private:
		utility::camera::Camera* pCamera;
		ParticleEmitter* pParticleEmitter;
		ColourInterpolator   colourInterpolator;

		ParticleBuffer      particles;
		VertexBuffer        vertexBuffer;
		glm::mat4x4         localToWorldMatrix;
		GLuint              textureID;

		// Apply this force to every particle in the effect
		glm::vec3           force;
	};
}
