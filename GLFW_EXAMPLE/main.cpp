    //Include GLEW  
    //#define GLEW_STATIC
	
	//Library for loading textures (Simple OpenGL Image Library)
	#include <SOIL.h>

    #include <GL/glew.h>  

	#include<iostream> //cout
	#include <fstream> //fstream

    //Include GLFW  
    #include <GLFW/glfw3.h>  
      
    //Include the standard C++ headers  
    #include <stdio.h>  
    #include <stdlib.h>  


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
			0.0f,  0.5f, // Vertex 1 (X, Y)
			0.5f, -0.5f, // Vertex 2 (X, Y)
			-0.5f, -0.5f  // Vertex 3 (X, Y)
		};


        // Example: generate vertex buffers
        GLuint buffer;
        glGenBuffers(1, &buffer);

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

        //TODO: load vertices and bind vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        //TODO: create and bind element buffer
       

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

		std::ifstream in1("shader.frag");
		std::string contents1((std::istreambuf_iterator<char>(in1)),
			std::istreambuf_iterator<char>());
		const char* fragmentSource = contents1.c_str();

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		glCompileShader(fragmentShader);

        //TODO: load and compile fragment shader shader.frag
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

        //TODO: link shaders into a program and bind outColor variable
		glBindFragDataLocation(shaderProgram, 0, "outColor");
		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);

        //TODO: link vertex data (position, colour and texture coords) to shader
		GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(posAttrib);

        //TODO: Create texture buffer:

        //TODO: Load image into texture buffer
        
        //TODO: Set texture parameters with glTexParameteri(...)

        //Set a background color  
        glClearColor(0.0f, 0.0f, 1.0f, 0.0f);  
      
        //Main Loop  
        do  
        {  
            //Clear color buffer  
            glClear(GL_COLOR_BUFFER_BIT); 
            
            //TODO: Draw the graphics
			glDrawArrays(GL_TRIANGLES, 0, 3);
            
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