#include <string>

//Define an error callback  
    static void error_callback(int error, const char* description)  
    {  
        fputs(description, stderr);  
        _fgetchar();  
    }  
      
    //Define the key input callback  
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)  
    {  
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)  
        glfwSetWindowShouldClose(window, GL_TRUE);  
    } 

    bool getShaderCompileStatus(GLuint shader){
        //Get status
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(status == GL_TRUE){
            return true;
        } else {
            //Get log
            char buffer[512];
            glGetShaderInfoLog(shader, 512, NULL, buffer);
            std::cout << buffer << std::endl;
            return false;
        }
    }
      
	// path relative to main? or where you are calling this. or using the shader. not relative to this file
	GLuint LoadShaders(std::string vertShader, std::string fragShader) {
		// Load the vertex shader from a file
		std::ifstream in(vertShader);
		std::string contents((std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>());

		// Load the fragment shader from a file
		std::ifstream in1(fragShader);
		std::string contents1((std::istreambuf_iterator<char>(in1)),
			std::istreambuf_iterator<char>());
		
		// Load and compile the vertex shader
		const char* vertSource = contents.c_str();
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertSource, NULL);
		glCompileShader(vertexShader);

		// Check that the shader compiled and print any errors
		getShaderCompileStatus(vertexShader);
		
		// Load and compile the fragment shader
		const char* fragSource = contents1.c_str();
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragSource, NULL);
		glCompileShader(fragmentShader);

		// Check that the shader compiled and print any errors
		getShaderCompileStatus(fragmentShader);

		// Link the shaders into the program
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		//glBindFragDataLocation(shaderProgram, 0, "outColor");
		glLinkProgram(shaderProgram);

		// Clean up the shaders
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return shaderProgram;
	}

	///<summary>Loads a cubemap from 6 texture files</summary>
	GLuint loadSkybox(std::vector<std::string> faces)
	{
		GLuint textureID;
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

		return textureID;
	}