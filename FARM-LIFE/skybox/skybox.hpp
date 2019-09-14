#ifndef A1_SKYBOX_HPP
#define A1_SKYBOX_HPP

namespace skybox {
	// Functions to initialize and render the skybox cubemap.
	class Skybox
	{
	public:
		// Member data, buffers and texture
		GLuint vao;
		GLuint shader;

		// Public functions

		// Constructor - builds the default skybox
		Skybox()
		{
			// Load the shader program used by the skybox
			shader = LoadShaders("skybox/shaders/skybox.vert", "skybox/shaders/skybox.frag");

			// Initialize the skybox vertices
			initSkybox();

			// Load the skybox textures
			loadTextures();
		}

		///<summary>
		/// Use the skybox shader program
		///</summary>
		void useShader()
		{
			glUseProgram(shader);
		}

		///<summary>
		/// Get integer uniform location
		///</summary>
		void getInt()
		{
			glUniform1i(glGetUniformLocation(shader, "skybox"), 0);
		}

		///<summary>
		/// Render the skybox, used in the main logic loop
		///</summary>
		void render(glm::mat4 view, glm::mat4 projection)
		{
			// view transforms
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content, won't draw skybox behind objects (optimization)
			glUseProgram(shader);
			glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);
			// render skybox cube
			glBindVertexArray(vao);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}


	private:
		// Private member data
		GLuint vbo;
		GLuint textureID;

		// Private functions

		///<summary>
		/// Initializes the skybox buffers and binds everything nicely.
		///</summary>
		void initSkybox()
		{
			float vertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};
			// Bind buffers for the skybox cubemesh
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		}

		///<summary>
		/// Loads the cubemap textures, requires 6 texture images to map to each cube face.
		///</summary>
		void loadTextures()
		{
			std::vector<std::string> faces
			{
				"skybox/textures/front.tga",
				"skybox/textures/back.tga",
				"skybox/textures/top.tga",
				"skybox/textures/bottom.tga",
				"skybox/textures/right.tga",
				"skybox/textures/left.tga",
			};

			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

			int width, height, nrComponents;
			for (unsigned int i = 0; i < faces.size(); i++)
			{
				unsigned char* data = SOIL_load_image(faces[i].c_str(), &width, &height, &nrComponents, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				}
				else
				{
					std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				}
				SOIL_free_image_data(data);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
	};

}

#endif A1_SKYBOX_HPP