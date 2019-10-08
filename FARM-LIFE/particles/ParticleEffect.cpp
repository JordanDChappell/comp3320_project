#include "ParticleSystemPCH.h"
#include "ParticleEmitter.h"
#include "Random.h"
#include "ParticleEffect.h"

namespace particle
{
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

		textureID = SOIL_load_OGL_texture(fileName.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

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

			float lifeRatio = glm::saturate(particle.fAge / particle.fLifeTime);
			particle.velocity += (force * fDeltaTime);
			particle.position += (particle.velocity * fDeltaTime);
			particle.colour = colourInterpolator.GetValue(lifeRatio);
			particle.fRotate = glm::lerp<float>(0.0f, 720.0f, lifeRatio);
			particle.fSize = glm::lerp<float>(5.0f, 0.0f, lifeRatio);
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
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &(vertexBuffer[0].pex0));
		glColorPointer(4, GL_FLOAT, sizeof(Vertex), &(vertexBuffer[0].diffuse));

		glDrawArrays(GL_QUADS, 0, vertexBuffer.size());

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		glBindTexture(GL_TEXTURE_2D, 0);

		glPopAttrib();

/*#if _DEBUG
		if (pParticleEmitter != NULL)
		{
			pParticleEmitter->DebugRender();
		}
#endif*/

		glPopMatrix();
	}
}