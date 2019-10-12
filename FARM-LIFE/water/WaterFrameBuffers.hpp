/* WaterFrameBuffers.hpp
 * Based on class by ThinMatrix on YouTube
 * Create and stores reflection, refraction and refraction depth textures for the water.
 */
#ifndef ASSIGNMENT_WATER_FRAME_BUFFERS_HPP
#define ASSIGNMENT_WATER_FRAME_BUFFERS_HPP

namespace water {
	
	class WaterFrameBuffers {
		public:
			// Buffer Constructor
			WaterFrameBuffers() {
				// Create frame buffer and texture attachment for reflection
				reflectionFrameBuffer = createFrameBuffer();
		        reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
		        reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
		        unbindCurrentFrameBuffer(1200, 800);

				// Create frame buffer and texture attachments for refraction and depth
        		refractionFrameBuffer = createFrameBuffer();
		        refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
		        refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
		        unbindCurrentFrameBuffer(1200, 800);
			}

			// Destructor
			~WaterFrameBuffers() {}

			// Call when closing the game
			void cleanup() {
		        glDeleteFramebuffers(1, &reflectionFrameBuffer);
		        glDeleteTextures(1, &reflectionTexture);
		        glDeleteRenderbuffers(1, &reflectionDepthBuffer);
		        glDeleteFramebuffers(1, &refractionFrameBuffer);
		        glDeleteTextures(1, &refractionTexture);
		        glDeleteTextures(1, &refractionDepthTexture);
		    }

			// Precondition: 
			// Postcondition: 	reflection frame buffer is bound
			void bindReflectionFrameBuffer() {
		        bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
		    }

			// Precondition:
			// Postcondition:	refraction frame buffer is bound
			void bindRefractionFrameBuffer() {
		        bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
		    }

			// Precondition:
			// Postcondition:	default frame buffer is bound
			void unbindCurrentFrameBuffer(GLuint SCREEN_WIDTH, GLuint SCREEN_HEIGHT) {
		        glBindFramebuffer(GL_FRAMEBUFFER, 0);
		        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		    }

			// Precondition:	reflection texture has been created
			// Postcondition:	returns reflection texture
			GLuint getReflectionTexture() {
		        return reflectionTexture;
		    }

			// Precondition:	refraction texture has been created
			// Postcondition:	returns refraction texture
			GLuint getRefractionTexture() {
		        return refractionTexture;
		    }

			// Precondition:	refraction depth texture has been created
			// Postcondition:	returns refraction depth texture
			GLuint getRefractionDepthTexture() {
		        return refractionDepthTexture;
		    }
 
		private:
			// Resolution of reflection and refraction textures 
			// Smaller resolution increases performance, larger looks more realistic
			static constexpr int REFLECTION_WIDTH = 600;
   			static constexpr int REFLECTION_HEIGHT = 400;
	     	static constexpr int REFRACTION_WIDTH = 600;
  			static constexpr int REFRACTION_HEIGHT = 400;
	
			// Store reflection objects
    		GLuint reflectionFrameBuffer;
    		GLuint reflectionTexture;
    		GLuint reflectionDepthBuffer;

			// Store refraction objects
			GLuint refractionFrameBuffer;
		    GLuint refractionTexture;
		    GLuint refractionDepthTexture;

		    // Precondition: 
		    // Postcondition: 	framebuffer is created
		    GLuint createFrameBuffer() {
		    	// Generate frame buffer
		        GLuint frameBuffer;
		        glGenFramebuffers(1, &frameBuffer);
		        // Bind the framebuffer
		        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		        // Indicate that we will always render to color attachment 0
		        glDrawBuffer(GL_COLOR_ATTACHMENT0);

		        return frameBuffer;
		    }

		    // Precondition: 	there is a buffer with no texture
		    // Postcondition: 	binds texture to the framebuffer
		    GLuint createTextureAttachment( int width, int height) {
		    	// Create texture
		    	GLuint texture;
		    	glGenTextures(1, &texture);

		    	// Bind texture
		    	glActiveTexture(GL_TEXTURE0);
		    	glBindTexture(GL_TEXTURE_2D, texture);
		        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, 
		        	GL_UNSIGNED_BYTE, 0);
		        
		        // Set texture parameters
		        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		        
		        // Set texture to framebuffer
		        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
		        
		        return texture;
		    }

		    // Precondition:	
		    // Postcondition: 	creates a depth buffer attachment on the active frame buffer
		    GLuint createDepthBufferAttachment(int width, int height) {
		    	// Create buffer
				GLuint depthBuffer;
				glGenRenderbuffers(1, &depthBuffer);
		        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
		        return depthBuffer;
    		}

    		// Precondition: 	
		    // Postcondition: 	binds creates depth buffer texture and binds to active frame buffer
    		GLuint createDepthTextureAttachment(int width, int height){
		        // Create texture
		        GLuint texture;
		        glGenTextures(1, &texture);
		        glBindTexture(GL_TEXTURE_2D, texture);
		        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		        // Set parameters
		        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
		        return texture;
		    }

			// Precondition:
			// Postcondition: 	binds the given frame buffer
			void bindFrameBuffer(int frameBuffer, int width, int height) {
				// Make sure a texture isn't bound
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
				glViewport(0, 0, width, height);
			}
	};
}

#endif