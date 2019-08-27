    //Include GLEW  
    //#define GLEW_STATIC
	
	//Library for loading textures (Simple OpenGL Image Library)
	#include <SOIL.h>

    #include <GL/glew.h>  

	#include <iostream> //cout
	#include <fstream> //fstream

    //Include GLFW  
    #include <GLFW/glfw3.h>  
      
    //Include the standard C++ headers  
    #include <stdio.h>  
    #include <stdlib.h>  

	#include "glm/glm.hpp"
	#include "glm/gtc/matrix_transform.hpp"
	#include "glm/gtc/type_ptr.hpp"


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
      
    int main( void )  
    {  
        //Set the error callback  
        glfwSetErrorCallback(error_callback);  
      
        //Initialize GLFW  
        if (!glfwInit())  
        {  
            exit(EXIT_FAILURE);  
        }  
      
        //Set the GLFW window creation hints - these are optional  
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version  
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); //Request a specific OpenGL version  
        //glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing  
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
      
        //Declare a window object  
        GLFWwindow* window;  
      
        //Create a window and create its OpenGL context  
        window = glfwCreateWindow(640, 480, "Test Window", NULL, NULL);  
      
        //If the window couldn't be created  
        if (!window)  
        {  
            fprintf( stderr, "Failed to open GLFW window.\n" );  
            glfwTerminate();  
            exit(EXIT_FAILURE);  
        }  
      
        //This function makes the context of the specified window current on the calling thread.   
        glfwMakeContextCurrent(window);  
      
        //Sets the key callback  
        glfwSetKeyCallback(window, key_callback);  
      
        //Initialize GLEW  
		glewExperimental = GL_TRUE;
        GLenum err = glewInit();  
      
        //If GLEW hasn't initialized  
        if (err != GLEW_OK)   
        {  
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err));  
            return -1;  
        }

        //TODO: create Vertex array object
		float vertices[] = {
			//  Position      Color             Texcoords
				-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
				 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
				 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
				-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
		};

        // Example: generate vertex buffers
        GLuint buffer;
        glGenBuffers(1, &buffer);

        //TODO: load vertices and bind vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        //TODO: create and bind element buffer
		

		GLuint ebo;
		glGenBuffers(1, &ebo);

		GLuint elements[] = {
			0, 1, 2,
			2, 3, 0
		};

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

        //Example:load shader source file
        std::ifstream in("shader.vert");
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
		std::ifstream in1("shader.frag");
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

        //TODO: link vertex data (position, colour and texture coords) to shader
		GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
			7 * sizeof(float), 0);
		
		GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
			7 * sizeof(float), (void*)(2 * sizeof(float)));

		GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
			7 * sizeof(float), (void*)(5 * sizeof(float)));
		
        //TODO: Create texture buffer:
		GLuint tex[2];
		glGenTextures(2, tex);
		
        //TODO: Load image into texture buffer
		//float pixels[] = {
		//	0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
		//	1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f
		//};
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
        
		int width, height;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[0]);

		unsigned char* image =
			SOIL_load_image("kitten.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

        //TODO: Set texture parameters with glTexParameteri(...)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glGenerateMipmap(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex[1]);
		image = SOIL_load_image("puppy.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



		//float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

        //Set a background color  
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  

		// GLint uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
        //Main Loop  
        do  
        {  
			// glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);

            //Clear color buffer  
            glClear(GL_COLOR_BUFFER_BIT); 

            //TODO: Draw the graphics
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
            //Swap buffers  
            glfwSwapBuffers(window);  
            //Get and organize events, like keyboard and mouse input, window resizing, etc...  
            glfwPollEvents();  
      
        } //Check if the ESC key had been pressed or if the window had been closed  
        while (!glfwWindowShouldClose(window));  
      
        //Close OpenGL window and terminate GLFW  
        glfwDestroyWindow(window);  
        //Finalize and clean up GLFW  
        glfwTerminate();  
      
        exit(EXIT_SUCCESS);  
    }  