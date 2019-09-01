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
		//Example:load shader source file
		std::ifstream in(vertShader);
		std::string contents((std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>());
		const char* vertSource = contents.c_str();
		//Example: compile a shader source file for vertex shading
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertSource, NULL);
		glCompileShader(vertexShader);

		//Example: check that the shader compiled and print any errors
		getShaderCompileStatus(vertexShader);

		//TODO: load and compile fragment shader shader.frag
		std::ifstream in1(fragShader);
		std::string contents1((std::istreambuf_iterator<char>(in1)),
			std::istreambuf_iterator<char>());
		const char* fragSource = contents1.c_str();

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragSource, NULL);
		glCompileShader(fragmentShader);
		getShaderCompileStatus(fragmentShader);

		//TODO: link shaders into a program and bind outColor variable
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

		glBindFragDataLocation(shaderProgram, 0, "outColor");

		glLinkProgram(shaderProgram);

		glUseProgram(shaderProgram);

		return shaderProgram;
	}