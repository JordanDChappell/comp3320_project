/* WaterFrameBuffers.hpp
 * Based on class by ThinMatrix on YouTube
 * Deals with the reflection and refraction textures for the water
 */
namespace water {
	
	class WaterFrameBuffers {
		public:
			// Buffer Constructor
			WaterFrameBuffers() {
				reflectionFrameBuffer = createFrameBuffer();
		        reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
		        reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
		        unbindCurrentFrameBuffer(1200, 800);

        		refractionFrameBuffer = createFrameBuffer();
		        refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
		        refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
		        unbindCurrentFrameBuffer(1200, 800);
			}

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

			// Call before rendering to reflection FBO
			void bindReflectionFrameBuffer() {
		        bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
		    }

		    // Call before rendering to refraction FBO
		    void bindRefractionFrameBuffer() {
		        bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
		    }

		    // Call to switch to default frame buffer
		    void unbindCurrentFrameBuffer(GLuint SCREEN_WIDTH, GLuint SCREEN_HEIGHT) {
		        glBindFramebuffer(GL_FRAMEBUFFER, 0);
		        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		    }

		 	// Get the reflection texture
		    GLuint getReflectionTexture() {
		        return reflectionTexture;
		    }
		     
		    // Get the refraction texture
		    GLuint getRefractionTexture() {
		        return refractionTexture;
		    }
		    
		    // Get the refraction depth texture
		    GLuint getRefractionDepthTexture() {
		        return refractionDepthTexture;
		    }
 
		private:
			static constexpr int REFLECTION_WIDTH = 320;
   			static constexpr int REFLECTION_HEIGHT = 180;
	     	static constexpr int REFRACTION_WIDTH = 1280;
  			static constexpr int REFRACTION_HEIGHT = 720;
 
    		GLuint reflectionFrameBuffer;
    		GLuint reflectionTexture;
    		GLuint reflectionDepthBuffer;
     
		    GLuint refractionFrameBuffer;
		    GLuint refractionTexture;
		    GLuint refractionDepthTexture;

		    // Precondition: 
		    // Postcondition: framebuffer is created
		    GLuint createFrameBuffer() {
		    	// Generate frame buffer
		        GLuint frameBuffer;
		        glGenFramebuffers(1, &frameBuffer);
		        //	Bind the framebuffer
		        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		        // Indicate that we will always render to color attachment 0
		        glDrawBuffer(GL_COLOR_ATTACHMENT0);

		        return frameBuffer;
		    }

		    // Precondition: there is a buffer with no texture
		    // Postcondition: binds texture to the framebuffer
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

		    // Precondition: there is a depth buffer on the frame buffer
		    // Postcondition: creates a depth buffer on the frame buffer
		    GLuint createDepthBufferAttachment(int width, int height) {
		    	// Create buffer
				GLuint depthBuffer;
				glGenRenderbuffers(1, &depthBuffer);
		        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
		        return depthBuffer;
    		}

    		// Precondition: there is a depth buffer with no texture
		    // Postcondition: binds texture to the depth buffer
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
	
			void bindFrameBuffer(int frameBuffer, int width, int height) {
				// Make sure a texture isn't bound
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
				glViewport(0, 0, width, height);
			}
	};
}