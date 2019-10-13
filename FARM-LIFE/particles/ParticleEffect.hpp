#ifndef ASSIGNMENT_PARTICLE_EFFECT_HPP
#define ASSIGNMENT_PARTICLE_EFFECT_HPP

#include "Interpolator.hpp"
#include "ParticleEmitter.hpp"
#include "../util/camera.hpp"
#include "Particle.hpp"
#include <GL/glew.h>
#include "SOIL.h"
#include "Random.hpp"
#include <glm/gtx/quaternion.hpp>
#include "glm/gtc/type_ptr.hpp"

namespace particle
{
    class ParticleEffect
	{
    public:
        // A vertex for the particle
		struct Vertex
		{
			glm::vec3 pos;		// Vertex position
			glm::vec4 diffuse;	// Diffuse colour
			glm::vec2 tex0;		// Texture coordinate

			Vertex() :
				pos(0),
				diffuse(0),
				tex0(0)
			{};
		};

		typedef std::vector<Vertex> VertexBuffer;
		typedef Interpolator<glm::vec4> ColourInterpolator;

		ParticleEffect(unsigned int numParticles = 0);
		virtual ~ParticleEffect();

		void SetCamera(utility::camera::Camera* camPos);
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

		Particle::ParticleBuffer      particles;
		VertexBuffer        vertexBuffer;
		glm::mat4x4         localToWorldMatrix;
		GLuint              textureID;

		// Apply this force to every particle in the effect
		glm::vec3           force;
	};

	ParticleEffect::ParticleEffect(unsigned int numParticles) :
		pCamera(NULL),
		pParticleEmitter(NULL),
		colourInterpolator(glm::vec4(1)),
		localToWorldMatrix(1),
		textureID(0),
		force(0, -9.81f, 0)
	{
		Resize(numParticles);
	}

	ParticleEffect::~ParticleEffect()
	{
		if (textureID != 0)
		{
			glDeleteTextures(1, &textureID);
			textureID = 0;
		}
	}

	void ParticleEffect::SetCamera(utility::camera::Camera* pCam)
	{
		pCamera = pCam;
	}

	void ParticleEffect::SetParticleEmitter(ParticleEmitter* pEmitter)
	{
		pParticleEmitter = pEmitter;
	}

	void ParticleEffect::SetColourInterplator(const ColourInterpolator& colors)
	{
		colourInterpolator = colors;
	}

	bool ParticleEffect::LoadTexture(const std::string& fileName)
	{
		if (textureID != 0)
		{
			glDeleteTextures(1, &textureID);
		}

		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = SOIL_load_image(fileName.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format = GL_RED;
			if (nrComponents == 1) format = GL_RED;
			else if (nrComponents == 3) format = GL_RGB;
			else if (nrComponents == 4) format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			SOIL_free_image_data(data);
		}
		else
		{
			std::cout << "(SOIL) Texture failed to load at path: " << fileName << std::endl;
			SOIL_free_image_data(data);
		}

		//textureID = SOIL_load_OGL_texture(fileName.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

		return (textureID != 0);
	}

	void ParticleEffect::RandomizeParticle(Particle& particle)
	{
		particle.fAge = 0.0f;
		particle.fLifeTime = RandRange(3, 5);

		glm::vec3 unitVec = RandUnitVec();

		particle.position = unitVec * 1.0f;
		particle.velocity = unitVec * RandRange(10, 20);
	}

	void ParticleEffect::RandomizeParticles()
	{
		for (unsigned int i = 0; i < particles.size(); ++i)
		{
			RandomizeParticle(particles[i]);
		}
	}

	void ParticleEffect::EmitParticle(Particle& particle)
	{
		assert(pParticleEmitter != NULL);
		pParticleEmitter->EmitParticle(particle);
	}

	void ParticleEffect::EmitParticles()
	{
		if (pParticleEmitter == NULL)
		{
			RandomizeParticles();
		}
		else
		{
			for (unsigned int i = 0; i < particles.size(); ++i)
			{
				EmitParticle(particles[i]);
			}
		}
	}

	void ParticleEffect::BuildVertexBuffer()
	{
		const glm::vec3 X(0.5, 0, 0);
		const glm::vec3 Y(0, 0.5, 0);
		const glm::vec3 Z(0, 0, 1.0);

		glm::quat cameraRotation;

		if (pCamera != NULL)
		{
			cameraRotation = glm::quat(glm::radians(pCamera->get_rotation()));
		}

		// Make sure the vertex buffer has enough vertices to render the effect
		// If the vertex buffer is already the correct size, no change is made.
		vertexBuffer.resize(particles.size() * 4, Vertex());

		for (unsigned int i = 0; i < particles.size(); ++i)
		{
			Particle& particle = particles[i];
			glm::quat rotation = glm::angleAxis(particle.fRotate, Z);

			unsigned int vertexIndex = i * 4;
			Vertex& v0 = vertexBuffer[vertexIndex + 0];   // Bottom-left
			Vertex& v1 = vertexBuffer[vertexIndex + 1];   // Bottom-right
			Vertex& v2 = vertexBuffer[vertexIndex + 2];   // Top-right
			Vertex& v3 = vertexBuffer[vertexIndex + 3];   // Top-left

			// Bottom-left
			v0.pos = particle.position + (rotation * (-X - Y) * particle.fSize) * cameraRotation;
			v0.tex0 = glm::vec2(0, 1);
			v0.diffuse = particle.colour;

			// Bottom-right
			v1.pos = particle.position + (rotation * (X - Y) * particle.fSize) * cameraRotation;
			v1.tex0 = glm::vec2(1, 1);
			v1.diffuse = particle.colour;

			// Top-right
			v2.pos = particle.position + (rotation * (X + Y) * particle.fSize) * cameraRotation;
			v2.tex0 = glm::vec2(1, 0);
			v2.diffuse = particle.colour;

			// Top-left
			v3.pos = particle.position + (rotation * (-X + Y) * particle.fSize) * cameraRotation;
			v3.tex0 = glm::vec2(0, 0);
			v3.diffuse = particle.colour;
		}

	}

	void ParticleEffect::Update(float fDeltaTime)
	{
		for (unsigned int i = 0; i < particles.size(); ++i)
		{
			Particle& particle = particles[i];

			particle.fAge += fDeltaTime;
			if (particle.fAge > particle.fLifeTime)
			{
				if (pParticleEmitter != NULL) EmitParticle(particle);
				else RandomizeParticle(particle);
			}

			float lifeRatio = glm::clamp(particle.fAge / particle.fLifeTime, 0.0f, 1.0f);
			particle.velocity += (force * fDeltaTime);
			particle.position += (particle.velocity * fDeltaTime);
			particle.colour = colourInterpolator.getValue(lifeRatio);
			particle.fRotate = glm::mix(0.0f, 720.0f, lifeRatio);
			particle.fSize = glm::mix(5.0f, 0.0f, lifeRatio);
		}

		BuildVertexBuffer();
	}

	void ParticleEffect::Render()
	{
		glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glDepthMask(GL_FALSE);            // Disable depth writes
		glEnable(GL_BLEND);                 // Enable Blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   // Type Of Blending To Perform
		glEnable(GL_TEXTURE_2D);            // Enable textures

		glPushMatrix();
		glMultMatrixf(glm::value_ptr(localToWorldMatrix));

		glBindTexture(GL_TEXTURE_2D, textureID);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(vertexBuffer[0].pos));
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &(vertexBuffer[0].tex0));
		glColorPointer(4, GL_FLOAT, sizeof(Vertex), &(vertexBuffer[0].diffuse));

		glDrawArrays(GL_QUADS, 0, vertexBuffer.size());

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		glBindTexture(GL_TEXTURE_2D, 0);

		glPopAttrib();
		glPopMatrix();
	}

	void ParticleEffect::Resize(unsigned int numParticles)
	{
		particles.resize(numParticles, Particle());
		vertexBuffer.resize(numParticles * 4, Vertex());
	}
}

#endif