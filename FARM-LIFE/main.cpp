    /* Add dependencies
	* SOIL - Library for loading textures (Simple OpenGL Image Library)
	* GLEW - OpenGL Extension Wrangler
	* GLFW - Graphics Library Framework
	* GLM - OpenGL Mathematics
	*/
	#include <SOIL.h>
    #include <GL/glew.h>  
	#include <GLFW/glfw3.h>  

	#include "glm/glm.hpp"
	#include "glm/gtc/matrix_transform.hpp"
	#include "glm/gtc/type_ptr.hpp"

	// Include the standard C++ headers  
	#include <iostream> 
	#include <fstream> 
    #include <stdio.h>  
    #include <stdlib.h>  
	#include <vector>

	// Include project files
	#include "util/mainUtil.hpp"
	#include "util/camera.hpp"

	// Initial width and height of the window
	static constexpr int SCREEN_WIDTH = 800;
	static constexpr int SCREEN_HEIGHT = 600;

	// Distances to the near and the far plane. Used for the camera to clip space transform.
	static constexpr float NEAR_PLANE = 0.1f;
	static constexpr float FAR_PLANE = 1000.0f;

	// Function to load skybox textures, or any cubemap textures
	//unsigned int loadSkybox(std::vector<std::string> faces);

	// Skybox vertices
	float skyboxVertices[] = {
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


	void process_input(GLFWwindow* window, const float& delta_time, utility::camera::Camera& camera) {
		camera.set_movement_sensitivity(0.005f * delta_time);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.move_forward();
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.move_backward();
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.move_left();
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.move_right();
		}
	}

    int main( void )  
    {  
		utility::camera::Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);

		//Set the error callback  
		glfwSetErrorCallback(error_callback);

		//Initialize GLFW  
		if (!glfwInit())
		{
			exit(EXIT_FAILURE);
		}

		//Set the GLFW window creation hints - these are optional  
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version  
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); //Request a specific OpenGL version  
		glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing  
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //modern opengl

		//Declare a window object  
		GLFWwindow* window;

		//Create a window and create its OpenGL context  
		window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Farm-Life: GOTY Edition", NULL, NULL);

		if (window == NULL) {
			std::cerr << "Failed to create GLFW window with dimension " << SCREEN_WIDTH << SCREEN_HEIGHT
				<< std::endl;
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window,
			CCallbackWrapper(GLFWframebuffersizefun, utility::camera::Camera)(
				std::bind(&utility::camera::Camera::framebuffer_size_callback,
					&camera,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3)));

		// get glfw to capture and hide the mouse pointer
		// ----------------------------------------------
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(
			window,
			CCallbackWrapper(GLFWcursorposfun, utility::camera::Camera)(std::bind(&utility::camera::Camera::mouse_callback,
				&camera,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3)));

		// get glfw to capture mouse scrolling
		// -----------------------------------
		glfwSetScrollCallback(
			window,
			CCallbackWrapper(GLFWscrollfun, utility::camera::Camera)(std::bind(&utility::camera::Camera::scroll_callback,
				&camera,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3)));

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

		// Enable depth test
		glEnable(GL_DEPTH_TEST);

		// Draw the triangle
		//TODO: create Vertex array object
		float vertices[] = {
			//  Position      Color
				-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
				 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
				 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
		};

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Example: generate vertex buffers
		GLuint buffer;
		glGenBuffers(1, &buffer);

		//TODO: load vertices and bind vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		GLuint shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

		GLuint skyboxShader = LoadShaders("shaders/skybox.vert", "shaders/skybox.frag");
		glUniform1i(glGetUniformLocation(skyboxShader, "skybox"), 0);

		// Draw the skybox
		// skybox VAO
		GLuint skyboxVAO, skyboxVBO;
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
		GLint skyAttrib = glGetAttribLocation(skyboxShader, "color");
		glEnableVertexAttribArray(skyAttrib);
		glVertexAttribPointer(skyAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // link vertex data (position, colour and texture coords) to shader
		/*GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
			5 * sizeof(float), 0);*/
		
		GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
			5 * sizeof(float), (void*)(2 * sizeof(float)));

		// Load skybox textures
		std::vector<std::string> faces
		{
			"textures/skybox/right.tga",
			"textures/skybox/left.tga",
			"textures/skybox/top.tga",
			"textures/skybox/bottom.tga",
			"textures/skybox/front.tga",
			"textures/skybox/back.tga",
		};

		GLuint skyboxTexture = loadSkybox(faces);

        // Set a background color  
        glClearColor(0.0f, 0.0f, 0.6f, 0.0f);  
		float last_frame = glfwGetTime();
		float delta_time = 0.0f;

        // Main Loop  
        do  
        {  
			float current_frame = glfwGetTime();
			float delta_time = current_frame - last_frame;
			float last_frame = current_frame;
			process_input(window, delta_time, camera);

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			// Clear color buffer  
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
			
			// Accept fragment if it closer to the camera than the former one
			/*glDepthFunc(GL_LESS);
			glUseProgram(shaderProgram);*/
			glm::mat4 Hvw = camera.get_view_transform();
			glm::mat4 Hcv = camera.get_clip_transform();
			glm::mat4 Hwm = glm::mat4(1.0f);
			/*glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Hvw"), 1, GL_FALSE, &Hvw[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Hcv"), 1, GL_FALSE, &Hcv[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Hwm"), 1, GL_FALSE, &Hwm[0][0]);
			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 3);*/
			//glBindVertexArray(0);

			// Draw the skybox
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
			glUseProgram(skyboxShader);
			Hvw = glm::mat4(glm::mat3(camera.get_view_transform())); // remove translation from the view matrix
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, &Hvw[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, &Hcv[0][0]);
			// skybox cube
			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//glBindVertexArray(0);
            
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